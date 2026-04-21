#!/usr/bin/env python3
import re
import random
from typing import Dict, List, Tuple

from pwn import remote
from z3 import BitVec, BitVecVal, LShR, Solver, sat
from cryptography.hazmat.primitives.ciphers.aead import AESGCM


# ============================================================
# Constants
# ============================================================
BLOCK_SIZE = 16
MASK64 = (1 << 64) - 1
MASK128 = (1 << 128) - 1
GCM_REDUCTION = 0xE1000000000000000000000000000000

N = 624
M = 397
MATRIX_A = 0x9908B0DF
WORD_MASK = 0xFFFFFFFF

QUERY_BUDGET = 704

# unknowns:
#   H: 128 bits
#   MT seeded-state: 624 * 32 bits
H_BITS = 128
MT_BITS = 624 * 32
TOTAL_BITS = H_BITS + MT_BITS


# ============================================================
# GF(2^128) / GHASH
# ============================================================
def gf_mul(x: int, y: int) -> int:
    z = 0
    v = x
    for i in range(128):
        if (y >> (127 - i)) & 1:
            z ^= v
        if v & 1:
            v = (v >> 1) ^ GCM_REDUCTION
        else:
            v >>= 1
    return z & MASK128


def length_block_for_one_block_message() -> int:
    return int.from_bytes((0).to_bytes(8, "big") + (16 * 8).to_bytes(8, "big"), "big")


LEN_BLOCK = length_block_for_one_block_message()


def ghash_low64_masks(cipher_block: int) -> List[int]:
    """
    Return 64 masks, LSB indexing:
      bit_b(low64(GHASH)) = parity(mask[b] & H_bits)
    where H is packed into variable bits [0..127].
    """
    masks = [0] * 64
    for j in range(128):
        basis = 1 << j
        val = gf_mul(cipher_block, gf_mul(basis, basis)) ^ gf_mul(LEN_BLOCK, basis)
        low = val & MASK64
        for b in range(64):
            if (low >> b) & 1:
                masks[b] |= 1 << j
    return masks


# ============================================================
# Symbolic MT19937 over GF(2)
# word bits are LSB-first
# ============================================================
def mt_var(word_idx: int, bit_idx: int) -> int:
    return 1 << (H_BITS + 32 * word_idx + bit_idx)


def make_word_from_mt_vars(word_idx: int) -> List[int]:
    return [mt_var(word_idx, b) for b in range(32)]


def xor_word(a: List[int], b: List[int]) -> List[int]:
    return [x ^ y for x, y in zip(a, b)]


def shr_word(a: List[int], k: int) -> List[int]:
    out = [0] * 32
    for i in range(32 - k):
        out[i] = a[i + k]
    return out


def shl_word(a: List[int], k: int) -> List[int]:
    out = [0] * 32
    for i in range(k, 32):
        out[i] = a[i - k]
    return out


def and_mask_word(a: List[int], mask: int) -> List[int]:
    return [a[i] if ((mask >> i) & 1) else 0 for i in range(32)]


def temper_word(x: List[int]) -> List[int]:
    y = x[:]
    y = xor_word(y, shr_word(y, 11))
    y = xor_word(y, and_mask_word(shl_word(y, 7), 0x9D2C5680))
    y = xor_word(y, and_mask_word(shl_word(y, 15), 0xEFC60000))
    y = xor_word(y, shr_word(y, 18))
    return y


def twist_state(state: List[List[int]]) -> List[List[int]]:
    """
    Exact CPython/MT19937 in-place twist.
    """
    st = [w[:] for w in state]

    for kk in range(N - M):
        y = [0] * 32
        for b in range(31):
            y[b] = st[kk + 1][b]
        y[31] = st[kk][31]

        yA = shr_word(y, 1)
        low = y[0]
        for b in range(32):
            if (MATRIX_A >> b) & 1:
                yA[b] ^= low

        st[kk] = xor_word(st[kk + M], yA)

    for kk in range(N - M, N - 1):
        y = [0] * 32
        for b in range(31):
            y[b] = st[kk + 1][b]
        y[31] = st[kk][31]

        yA = shr_word(y, 1)
        low = y[0]
        for b in range(32):
            if (MATRIX_A >> b) & 1:
                yA[b] ^= low

        st[kk] = xor_word(st[kk + (M - N)], yA)

    y = [0] * 32
    for b in range(31):
        y[b] = st[0][b]
    y[31] = st[N - 1][31]

    yA = shr_word(y, 1)
    low = y[0]
    for b in range(32):
        if (MATRIX_A >> b) & 1:
            yA[b] ^= low

    st[N - 1] = xor_word(st[M - 1], yA)
    return st


class SymbolicMT:
    def __init__(self):
        self.state = [make_word_from_mt_vars(i) for i in range(N)]
        self.index = N  # Python stores index=624 right after seed()

    def getrandbits32_symbolic(self) -> List[int]:
        if self.index >= N:
            self.state = twist_state(self.state)
            self.index = 0
        y = temper_word(self.state[self.index])
        self.index += 1
        return y


# ============================================================
# GF(2) solver
# ============================================================
class GF2Solver:
    def __init__(self, nvars: int):
        self.nvars = nvars
        self.pivots: Dict[int, Tuple[int, int]] = {}

    def add_equation(self, row: int, rhs: int):
        x = row
        y = rhs & 1
        while x:
            p = x.bit_length() - 1
            if p in self.pivots:
                prow, prhs = self.pivots[p]
                x ^= prow
                y ^= prhs
            else:
                self.pivots[p] = (x, y)
                return
        if y:
            raise ValueError("Inconsistent linear system")

    def solve(self) -> List[int]:
        sol = [0] * self.nvars
        packed = 0

        # Important: solve from low pivot to high pivot
        for p in sorted(self.pivots.keys()):
            row, rhs = self.pivots[p]
            rest = row ^ (1 << p)
            bit = rhs ^ ((rest & packed).bit_count() & 1)
            sol[p] = bit
            if bit:
                packed |= 1 << p

        return sol

    def check_solution(self, sol: List[int]) -> bool:
        packed = 0
        for i, b in enumerate(sol):
            if b:
                packed |= 1 << i
        for _, (row, rhs) in self.pivots.items():
            if ((row & packed).bit_count() & 1) != rhs:
                return False
        return True


# ============================================================
# Remote IO
# ============================================================
def recv_menu(io) -> bytes:
    return io.recvuntil(b"> ")


def parse_initial(blob: bytes):
    s = blob.decode()
    flag_nonce = bytes.fromhex(re.search(r"flag_nonce = ([0-9a-f]+)", s).group(1))
    flag_ct = bytes.fromhex(re.search(r"flag_ciphertext = ([0-9a-f]+)", s).group(1))
    flag_tag = bytes.fromhex(re.search(r"flag_tag = ([0-9a-f]+)", s).group(1))
    budget = int(re.search(r"query budget = (\d+)", s).group(1))
    return flag_nonce, flag_ct, flag_tag, budget


def collect_all_queries_fast(io, n=QUERY_BUDGET):
    io.send(b"1\n" * n + b"2\n")
    data = io.recvall(timeout=20).decode()

    matches = re.findall(
        r"nonce = ([0-9a-f]+)\n"
        r"ciphertext = ([0-9a-f]+)\n"
        r"tag = ([0-9a-f]+)\n"
        r"queries_left = (\d+)",
        data,
    )

    if len(matches) != n:
        raise RuntimeError(f"expected {n} queries, got {len(matches)}")

    queries = []
    for _nonce_hex, ct_hex, tag_hex, _left in matches:
        queries.append((bytes.fromhex(ct_hex), bytes.fromhex(tag_hex)))

    return queries


# ============================================================
# Recover H and seeded MT state
# ============================================================
def recover_h_and_mt_state(queries: List[Tuple[bytes, bytes]]) -> Tuple[int, List[int]]:
    solver = GF2Solver(TOTAL_BITS)
    smt = SymbolicMT()

    for idx, (ct, tag) in enumerate(queries):
        c = int.from_bytes(ct, "big")
        g_masks = ghash_low64_masks(c)

        out0 = smt.getrandbits32_symbolic()
        out1 = smt.getrandbits32_symbolic()

        low = int.from_bytes(tag, "big") & MASK64

        for b in range(64):
            row = g_masks[b]

            # fault_value = (out0 << 32) | out1
            # with LSB indexing on low64:
            #   bit 0..31   -> out1[0..31]
            #   bit 32..63  -> out0[0..31]
            if b < 32:
                row ^= out1[b]
            else:
                row ^= out0[b - 32]

            rhs = (low >> b) & 1
            solver.add_equation(row, rhs)

        if (idx + 1) % 64 == 0:
            print(f"[+] processed {idx + 1}/{len(queries)} queries")

    # CPython post-seed invariant
    for b in range(32):
        solver.add_equation(mt_var(0, b), (0x80000000 >> b) & 1)

    print("[+] solving linear system...")
    sol = solver.solve()
    assert solver.check_solution(sol), "linear solution does not satisfy the system"

    H = 0
    for j in range(128):
        if sol[j]:
            H |= 1 << j

    mt_state = []
    base = H_BITS
    for i in range(624):
        x = 0
        for b in range(32):
            if sol[base + 32 * i + b]:
                x |= 1 << b
        mt_state.append(x)

    return H, mt_state


def check_mt_state_recovery(mt_state: List[int], queries: List[Tuple[bytes, bytes]], H: int, count: int = 8):
    r = random.Random()
    r.setstate((3, tuple(mt_state + [624]), None))

    ok_all = True
    for i, (ct, tag) in enumerate(queries[:count]):
        out0 = r.getrandbits(32)
        out1 = r.getrandbits(32)
        fault = ((out0 << 32) | out1) & MASK64

        c = int.from_bytes(ct, "big")
        ghash = gf_mul(c, gf_mul(H, H)) ^ gf_mul(LEN_BLOCK, H)
        expected = (ghash ^ fault) & MASK64
        actual = int.from_bytes(tag, "big") & MASK64
        ok = expected == actual
        ok_all &= ok
        print(f"[chk] {i}: expected={expected:016x} actual={actual:016x} ok={ok}")

    if not ok_all:
        raise RuntimeError("MT/H sanity check failed")


# ============================================================
# Reverse Python seed(int) with z3
# ============================================================
def init_genrand_19650218() -> List[int]:
    mt = [0] * N
    mt[0] = 19650218
    for i in range(1, N):
        mt[i] = (1812433253 * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i) & WORD_MASK
    return mt


INIT_19650218 = init_genrand_19650218()


def python_state_from_seed_words(words_le: List[int]) -> List[int]:
    seed_int = 0
    for i, w in enumerate(words_le):
        seed_int |= (w & WORD_MASK) << (32 * i)
    r = random.Random(seed_int)
    return list(r.getstate()[1][:-1])


def recover_seed_words_with_z3(target_state: List[int], max_words: int = 8) -> List[int]:
    print("[+] recovering seed words with z3...")

    for key_words in range(1, max_words + 1):
        print(f"[+] trying key_words = {key_words}")

        key = [BitVec(f"k_{key_words}_{i}", 32) for i in range(key_words)]
        mt = [BitVecVal(x, 32) for x in INIT_19650218]

        i = 1
        j = 0
        k = max(N, key_words)

        # init_by_array first loop
        for _ in range(k):
            mt[i] = (
                mt[i]
                ^ ((mt[i - 1] ^ LShR(mt[i - 1], 30)) * BitVecVal(1664525, 32))
            ) + key[j] + BitVecVal(j, 32)

            i += 1
            j += 1

            if i >= N:
                mt[0] = mt[N - 1]
                i = 1
            if j >= key_words:
                j = 0

        # second loop
        for _ in range(N - 1):
            mt[i] = (
                mt[i]
                ^ ((mt[i - 1] ^ LShR(mt[i - 1], 30)) * BitVecVal(1566083941, 32))
            ) - BitVecVal(i, 32)

            i += 1
            if i >= N:
                mt[0] = mt[N - 1]
                i = 1

        mt[0] = BitVecVal(0x80000000, 32)

        s = Solver()
        for idx in range(N):
            s.add(mt[idx] == BitVecVal(target_state[idx], 32))

        if s.check() != sat:
            continue

        model = s.model()
        words = [model[k].as_long() & WORD_MASK for k in key]

        # forward validation against real CPython
        if python_state_from_seed_words(words) == target_state:
            print(f"[+] found valid seed with key_words = {key_words}")
            return words

    raise RuntimeError("z3 failed to recover seed words for all key lengths 1..8")


def words_to_key(words_le: List[int], key_len: int = 32) -> bytes:
    seed_int = 0
    for i, w in enumerate(words_le):
        seed_int |= (w & WORD_MASK) << (32 * i)
    return seed_int.to_bytes(key_len, "big")


# ============================================================
# Decrypt
# ============================================================
def decrypt_flag(master_key: bytes, nonce: bytes, ct: bytes, tag: bytes) -> bytes:
    return AESGCM(master_key).decrypt(nonce, ct + tag, None)


# ============================================================
# Main
# ============================================================
def main():
    io = remote("manytags.opus4-7.b01le.rs", 8443, ssl=True)

    banner = recv_menu(io)
    flag_nonce, flag_ct, flag_tag, budget = parse_initial(banner)

    print(f"[+] budget = {budget}")
    print(f"[+] flag_nonce = {flag_nonce.hex()}")
    print(f"[+] flag_ct = {flag_ct.hex()}")
    print(f"[+] flag_tag = {flag_tag.hex()}")

    assert budget == QUERY_BUDGET

    queries = collect_all_queries_fast(io, budget)
    print(f"[+] collected {len(queries)} queries")

    H, mt_state = recover_h_and_mt_state(queries)
    print(f"[+] H = {H:032x}")

    check_mt_state_recovery(mt_state, queries, H, 8)

    seed_words = recover_seed_words_with_z3(mt_state, 8)
    print("[+] recovered seed words:")
    for i, w in enumerate(seed_words):
        print(f"    w[{i}] = {w:08x}")

    master_key = words_to_key(seed_words, 32)
    print(f"[+] master_key = {master_key.hex()}")

    pt = decrypt_flag(master_key, flag_nonce, flag_ct, flag_tag)
    print(f"[+] flag = {pt!r}")


if __name__ == "__main__":
    main()