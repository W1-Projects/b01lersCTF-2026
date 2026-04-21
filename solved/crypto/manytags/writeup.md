# too many tags

**Category:** Crypto

## Description
> No standalone organizer description was included in the provided artifacts.

## Overview
The challenge is a key-recovery attack against an intentionally broken AES-GCM service. The core flaw is that the 32-byte `master_key` is reused as the seed of Python's `random.Random`, and each query leaks `low64(GHASH_H(C)) XOR fault_value` for a one-block ciphertext, where `fault_value` is built from two consecutive MT19937 outputs. `solve.py` exploits the fact that both one-block GHASH and MT19937 are linear over GF(2), recovers the seeded PRNG state, inverts CPython's seeding routine to reconstruct `master_key`, and decrypts the flag with the real GCM tag printed at startup.

```
#!/usr/bin/env python3
import random
import secrets

from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

BLOCK_SIZE = 16
BLOCK_BITS = 8 * BLOCK_SIZE
MASK64 = (1 << 64) - 1
MASK128 = (1 << BLOCK_BITS) - 1
GCM_REDUCTION = 0xE1000000000000000000000000000000
STATE_SIZE = 624
QUERY_BUDGET = 704
WORD_MASK = 0xFFFFFFFF

master_key = secrets.token_bytes(32)
used = 0


def new_fault_rng(key):
    return random.Random(int.from_bytes(key, "big"))


mask_rng = new_fault_rng(master_key)


def aes_block_encrypt(key, block):
    encryptor = Cipher(algorithms.AES(key), modes.ECB()).encryptor()
    return encryptor.update(block) + encryptor.finalize()


def aes_gcm_encrypt(key, nonce, message):
    encrypted = AESGCM(key).encrypt(nonce, message, None)
    return encrypted[:-BLOCK_SIZE], encrypted[-BLOCK_SIZE:]


def compute_h(key):
    return aes_block_encrypt(key, b"\x00" * BLOCK_SIZE)


def gf_mul(x, y):
    z = 0
    v = x
    for i in range(BLOCK_BITS):
        if (y >> (127 - i)) & 1:
            z ^= v
        if v & 1:
            v = (v >> 1) ^ GCM_REDUCTION
        else:
            v >>= 1
    return z & MASK128


def ghash(subkey, blocks):
    tag = 0
    for block in blocks:
        tag = gf_mul(tag ^ block, subkey)
    return tag


def compute_length_block(message_len):
    return (0).to_bytes(8, "big") + (message_len * 8).to_bytes(8, "big")


def compute_one_block_ghash(h_int, ciphertext):
    return ghash(
        h_int,
        [
            int.from_bytes(ciphertext, "big"),
            int.from_bytes(compute_length_block(len(ciphertext)), "big"),
        ],
    )


def compute_gcm_mask(key, nonce):
    return aes_block_encrypt(key, nonce + b"\x00\x00\x00\x01")


def construct_tag(real_tag, ghash_value, fault_words):
    real_tag_int = int.from_bytes(real_tag, "big")
    fault_value = (fault_words[0] << 32) | fault_words[1]
    tag_low = (ghash_value ^ fault_value) & MASK64
    tag_int = (real_tag_int & (~MASK64)) | tag_low
    return tag_int.to_bytes(BLOCK_SIZE, "big")


def encrypt_query_message(key, nonce, message, fault_words):
    ciphertext, real_tag = aes_gcm_encrypt(key, nonce, message)
    h_int = int.from_bytes(compute_h(key), "big")
    ghash_value = compute_one_block_ghash(h_int, ciphertext)
    tag = construct_tag(real_tag, ghash_value, fault_words)
    return {
        "ciphertext": ciphertext,
        "real_tag": real_tag,
        "tag": tag,
        "ghash": ghash_value.to_bytes(BLOCK_SIZE, "big"),
    }


def next_fault_words(rng):
    return [rng.getrandbits(32), rng.getrandbits(32)]


def encrypt_query_with_rng(key, nonce, message, rng):
    fault_words = next_fault_words(rng)
    result = encrypt_query_message(key, nonce, message, fault_words)
    result["fault_words"] = fault_words
    return result


def encrypt_flag_message(key, nonce, message):
    return aes_gcm_encrypt(key, nonce, message)


def get_tag():
    global used

    if used >= QUERY_BUDGET:
        print("No more tags for you")
        return

    nonce = secrets.token_bytes(12)
    message = secrets.token_bytes(BLOCK_SIZE)
    result = encrypt_query_with_rng(master_key, nonce, message, mask_rng)
    used += 1
    print(f"nonce = {nonce.hex()}")
    print(f"ciphertext = {result['ciphertext'].hex()}")
    print(f"tag = {result['tag'].hex()}")
    print(f"queries_left = {QUERY_BUDGET - used}")


def main():
    with open("./flag.txt") as f:
        flag = f.read().strip().encode()

    flag_nonce = secrets.token_bytes(12)
    flag_cipher, flag_tag = encrypt_flag_message(master_key, flag_nonce, flag)

    print(
        f"""
*********************************************************

too many tags

*********************************************************

flag_nonce = {flag_nonce.hex()}
flag_ciphertext = {flag_cipher.hex()}
flag_tag = {flag_tag.hex()}

query budget = {QUERY_BUDGET}

************************* Menu **************************

1. Get a random ciphertext and tag
2. Exit

*********************************************************
"""
    )

    while True:
        try:
            option = input("> ")
        except EOFError:
            return

        if option == "1":
            get_tag()
        elif option == "2":
            print("Bye")
            return
        else:
            print("Invalid option")


if __name__ == "__main__":
    main()

```

## Technical details
The root cause is state/key reuse across two unrelated primitives:

```python
master_key = secrets.token_bytes(32)

def new_fault_rng(key):
    return random.Random(int.from_bytes(key, "big"))

mask_rng = new_fault_rng(master_key)
```

`master_key` is simultaneously the AES-GCM key and the seed of MT19937. The attacker cannot choose plaintexts directly, but can repeatedly trigger the leaky path by selecting menu option `1`:

```python
def get_tag():
    nonce = secrets.token_bytes(12)
    message = secrets.token_bytes(BLOCK_SIZE)
    result = encrypt_query_with_rng(master_key, nonce, message, mask_rng)
    print(f"ciphertext = {result['ciphertext'].hex()}")
    print(f"tag = {result['tag'].hex()}")
```

Each query uses a fixed one-block message length (`BLOCK_SIZE = 16`), which is critical because it collapses GHASH to a small algebraic form. The service first computes a valid GCM tag, then overwrites its low 64 bits:

```python
def construct_tag(real_tag, ghash_value, fault_words):
    fault_value = (fault_words[0] << 32) | fault_words[1]
    tag_low = (ghash_value ^ fault_value) & MASK64
    tag_int = (real_tag_int & (~MASK64)) | tag_low
    return tag_int.to_bytes(BLOCK_SIZE, "big")
```

For a one-block ciphertext `C` and length block `L = 0^64 || 128`, the leaked part is:

```text
low64(tag) = low64(GHASH_H(C)) XOR ((out0 << 32) | out1)
GHASH_H(C) = C * H^2 XOR L * H
H = AES_K(0^128)
```

This is exploitable because the leak is linear in the unknowns.

1. `C * H^2 XOR L * H` is linear in the 128 bits of `H` over GF(2). `solve.py` materializes this with `ghash_low64_masks(cipher_block)`, which computes 64 masks such that each leaked GHASH bit is the parity of a mask against the 128 symbolic `H` bits.
2. MT19937 output is also linear in the seeded state when viewed bitwise over GF(2): `temper_word`, `twist_state`, shifts, XORs, and masked shifts are all linear operations. `solve.py` models the seeded state as `624 * 32` symbolic bits and sets `self.index = N` to match CPython's post-seed behavior, where the first `getrandbits(32)` twists before output.

The unknown vector is therefore:

```text
X = H_bits (128) || seeded_MT_state_bits (624 * 32)
total = 128 + 624*32 = 20096 bits
```

Every query yields 64 independent linear equations because only the low 64 tag bits are faulted. `solve.py` wires the two halves together exactly as produced by the service:

```python
out0 = smt.getrandbits32_symbolic()
out1 = smt.getrandbits32_symbolic()

for b in range(64):
    row = g_masks[b]
    if b < 32:
        row ^= out1[b]
    else:
        row ^= out0[b - 32]
    solver.add_equation(row, (low >> b) & 1)
```

With `QUERY_BUDGET = 704`, the attacker gets `704 * 64 = 45056` equations, well above 20096 unknowns. `solve.py` also adds the CPython seeding invariant `mt[0] = 0x80000000`, then performs Gaussian elimination in `GF2Solver`. This recovers:

- a consistent 128-bit representation of the GHASH subkey used by the linear model;
- the full 624-word MT19937 state immediately after seeding.

That state is sufficient to recover the AES key because the seed was `int.from_bytes(master_key, "big")`. `recover_seed_words_with_z3()` reimplements CPython's `init_by_array` with Z3:

```python
for key_words in range(1, max_words + 1):
    key = [BitVec(f"k_{key_words}_{i}", 32) for i in range(key_words)]
    ...
    mt[0] = BitVecVal(0x80000000, 32)
    for idx in range(N):
        s.add(mt[idx] == BitVecVal(target_state[idx], 32))
```

The solver searches seed lengths `1..8` words and validates candidates by forwarding them through real CPython state generation:

```python
if python_state_from_seed_words(words) == target_state:
    return words
```

For this challenge, the correct solution is 8 words = 256 bits, exactly matching the 32-byte AES key. `words_to_key()` converts those little-endian 32-bit seed words back into the original big-endian `master_key`. The final flag decryption succeeds because the startup banner uses the honest GCM path:

```python
flag_cipher, flag_tag = encrypt_flag_message(master_key, flag_nonce, flag)
```

No forgery is needed after key recovery; the leaked PRNG state is enough to reconstruct the AES-GCM key itself.

## Proof-of-Concept
- Step 1: Exhaust the oracle budget and collect all `(ciphertext, tag)` samples. The attacker-controlled action is only repeated selection of menu option `1`, but that is sufficient because each response provides a known ciphertext block and a 64-bit linear leak.

```python
io.send(b"1\n" * n + b"2\n")
matches = re.findall(
    r"ciphertext = ([0-9a-f]+)\n"
    r"tag = ([0-9a-f]+)\n",
    io.recvall(timeout=20).decode(),
)
```

Artifact: 704 observed one-block ciphertexts and corrupted tags. These samples become the coefficients and right-hand sides of the linear system.

- Step 2: Build one 20096-variable GF(2) system that simultaneously models GHASH and MT19937 output. `ghash_low64_masks()` turns each ciphertext block into 64 masks over the 128 GHASH-subkey bits, while `SymbolicMT` yields the symbolic bits of the two PRNG outputs consumed by that query.

```python
g_masks = ghash_low64_masks(int.from_bytes(ct, "big"))
out0 = smt.getrandbits32_symbolic()
out1 = smt.getrandbits32_symbolic()
solver.add_equation(row, (low >> b) & 1)
```

Artifact: a solved linear system giving the seeded 624-word MT state and a consistent GHASH-subkey representation. `check_mt_state_recovery()` confirms the result by recomputing the first few leaked low 64-bit tags from the recovered state.

- Step 3: Invert CPython seeding to recover the original seed words. The recovered MT state is constrained against a Z3 model of `init_by_array`, trying key lengths from 1 to 8 words.

```python
seed_words = recover_seed_words_with_z3(mt_state, 8)
master_key = words_to_key(seed_words, 32)
```

Artifact: the exact 32-byte `master_key`. This works because the service seeded MT with `int.from_bytes(master_key, "big")`, so recovering the PRNG seed is equivalent to recovering the AES key.

- Step 4: Decrypt the startup flag ciphertext with the recovered key. The flag path uses a real AES-GCM tag, so standard decryption is enough.

```python
pt = AESGCM(master_key).decrypt(flag_nonce, flag_ct + flag_tag, None)
```

Artifact: the plaintext flag. This is the final win condition; once `master_key` is known, the challenge reduces to ordinary AES-GCM decryption.

## P/S
The exploit depends on two implementation details that are easy to miss. First, the query messages are always exactly one block long, which is what makes `GHASH_H(C) = C * H^2 XOR L * H` compact enough to linearize cleanly. Second, Python's `random.Random` stores `index = 624` immediately after seeding, so the first `getrandbits(32)` in the oracle corresponds to a post-seed twist; `solve.py` mirrors that exactly by initializing `SymbolicMT.index = N`.
