from __future__ import annotations

import re
from pathlib import Path

def match_at(buf: bytes, pos: int, pat: list[int | None]):
    if pos + len(pat) > len(buf):
        return None
    vals = []
    for i, b in enumerate(pat):
        cur = buf[pos + i]
        if b is None:
            vals.append(cur)
        elif cur != b:
            return None
    return vals


ADD_CONST = [0x08, 0x18, 0x69, None, 0x28]
XOR_CONST = [0x08, 0x49, None, 0x28]
SWAP_AX = [0x08, 0x48, 0x8A, 0x48, 0xBA, 0xE8, 0x9A, 0x68, 0xCA, 0x9A, 0xAA, 0x68, 0x28, 0x28]
ADD_A_TO_X = [0x08, 0x48, 0xE8, 0x38, 0xE9, 0x01, 0xD0, 0xFA, 0x68, 0x28]
ROR_A = [0x08, 0x48, 0x8A, 0x48, 0xBA, 0xE8, 0x9A, 0x68, 0xA2, None, 0x4A, 0x90, 0x02, 0x09, 0x80, 0xCA, 0xD0, 0xF8, 0x48, 0xBA, 0xCA, 0x9A, 0x68, 0xAA, 0x68, 0x28]
SWAP_AY = [0x08, 0x48, 0x98, 0x48, 0x8A, 0x48, 0xBA, 0xE8, 0xE8, 0x9A, 0x68, 0xA8, 0xCA, 0xCA, 0x9A, 0x68, 0xAA, 0x68, 0x28, 0x28]
XOR_A_Y = [0x08, 0x48, 0x98, 0x48, 0x8A, 0x48, 0xBA, 0xE8, 0x9A, 0x68]
for bit in [1, 2, 4, 8, 16, 32, 64, 128]:
    XOR_A_Y += [0x4A, 0x90, 0x06, 0xA8, 0x68, 0x49, bit, 0x48, 0x98]
XOR_A_Y += [0xCA, 0x9A, 0x68, 0xAA, 0x68, 0xA8, 0x68, 0x28]
SWAP_XY = [0x08, 0x48, 0x8A, 0x48, 0x98, 0xAA, 0x68, 0xA8, 0x68, 0x28]
ADD_A_TO_Y = [0x08, 0x48, 0xC8, 0x38, 0xE9, 0x01, 0xD0, 0xFA, 0x68, 0x28]

PATTERNS = [
    ("ADD_CONST", ADD_CONST),
    ("XOR_CONST", XOR_CONST),
    ("SWAP_AX", SWAP_AX),
    ("ADD_A_TO_X", ADD_A_TO_X),
    ("ROR_A", ROR_A),
    ("SWAP_AY", SWAP_AY),
    ("XOR_A_Y", XOR_A_Y),
    ("SWAP_XY", SWAP_XY),
    ("ADD_A_TO_Y", ADD_A_TO_Y),
]


def parse_macros(buf: bytes):
    pos = 0
    ops: list[tuple[str, int | None]] = []
    while pos < len(buf):
        if pos == len(buf) - 1 and buf[pos] == 0x60:  # final RTS
            break
        ok = False
        for name, pat in PATTERNS:
            vals = match_at(buf, pos, pat)
            if vals is None:
                continue
            ops.append((name, vals[0] if vals else None))
            pos += len(pat)
            ok = True
            break
        if not ok:
            raise ValueError(f"unknown sequence at offset 0x{pos:x}: {buf[pos:pos+32].hex()}")
    return ops


def rol8(x: int, n: int) -> int:
    n &= 7
    if n == 0:
        return x & 0xFF
    return (((x << n) & 0xFF) | (x >> (8 - n))) & 0xFF


def invert(ops, a: int, x: int, y: int):
    for name, v in reversed(ops):
        if name == "ADD_CONST":
            a = (a - v) & 0xFF
        elif name == "XOR_CONST":
            a ^= v
        elif name == "SWAP_AX":
            a, x = x, a
        elif name == "SWAP_AY":
            a, y = y, a
        elif name == "SWAP_XY":
            x, y = y, x
        elif name == "ADD_A_TO_X":
            x = (x - a) & 0xFF
        elif name == "ADD_A_TO_Y":
            y = (y - a) & 0xFF
        elif name == "ROR_A":
            a = rol8(a, v)
        elif name == "XOR_A_Y":
            a ^= y
        else:
            raise ValueError(name)
    return a, x, y


def solve_lines(text: str, ops):
    ans = []
    for a, x, y in re.findall(r"A=(\d+)\s+X=(\d+)\s+Y=(\d+)", text):
        ans.append(invert(ops, int(a), int(x), int(y)))
    return ans


def main():
    blob = Path("code-10k.bin").read_bytes()
    ops = parse_macros(blob)

    print("Paste the challenge output, then Ctrl-D:")
    text = __import__("sys").stdin.read()
    triples = solve_lines(text, ops)
    for i, (a, x, y) in enumerate(triples, 1):
        print(f"Input #{i}: {a},{x},{y}")


if __name__ == "__main__":
    main()