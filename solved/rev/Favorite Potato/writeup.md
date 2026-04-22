# Potato

**Category:** Rev

## Description

> Yay, at last - I managed to upgrade my old potato. Now I can run suuuuper loooong binaries that nobody can reverse (RAM is still 64k but I only need a minimal amount).
>
> Flag format: `bctf{...}`

## Overview

This is a reverse engineering challenge built around a very large 6502 binary. At first glance the intended difficulty is the size of the program: the attached `code.bin` is several megabytes long, and the description suggests that the binary is too long to analyze manually.

However, the Python wrapper immediately reveals the real task. The challenge does **not** ask us to extract a flag directly from the binary. Instead, it repeatedly does the following:

1. Generate a random 3-byte input `(A0, X0, Y0)`.
2. Execute the 6502 binary with those values as the initial register state.
3. Print the final register state `(A, X, Y)`.
4. Ask the player to recover the original `(A0, X0, Y0)` values.

In real mode this happens 20 times. If we recover all 20 inputs correctly, the wrapper prints the contents of `flag.txt`.

So the core problem is not “understand the whole huge binary instruction by instruction,” but rather:

- model the transformation
- prove it is invertible
- write a script that takes the printed outputs and reconstructs the original inputs

The key insight is that the binary is enormous only because it is composed of the same few reversible 6502 instruction macros repeated thousands of times.

## Technical details

The provided wrapper makes the solve strategy clear:

```python
import os
from helpers import run_c64

ROUNDS = 20

def randomAXY():
    return os.urandom(3)

def singleEval(binary, msg):
    A0, X0, Y0 = randomAXY()
    A, X, Y = run_c64(binary, A0, X0, Y0)
    print(f"{msg}: A={A} X={X} Y={Y}")
    return [str(v) for v in (A0, X0, Y0)]
```

That means `run_c64("code.bin", A0, X0, Y0)` behaves like a pure function from three bytes to three bytes. There is no persistent state between rounds. Therefore, if we can invert the transformation once, we can solve all 20 rounds automatically.

### 1. Looking at the disassembly

The beginning of the radare2 dump already shows the patterns that dominate the whole binary:

- `adc #imm` wrapped by `php/plp` → add an immediate constant to `A`
- stack/register shuffling with `pha`, `pla`, `txa`, `tya`, `tax`, `tay`, `tsx`, `txs` → register swaps
- loops like `inx ; sec ; sbc #1 ; bne ...` → add the old `A` value into `X`
- loops like `iny ; sec ; sbc #1 ; bne ...` → add the old `A` value into `Y`
- repeated `lsr` and conditional `ora #0x80` → rotate `A` right by a chosen amount
- repeated `lsr` and conditional `eor bit` while preserving `Y` → `A ^= Y`

So while the binary is very long, its behavior can be compressed into a short set of high-level macro operations:

- `ADD_CONST(c)`
- `XOR_CONST(c)`
- `SWAP_AX`
- `SWAP_AY`
- `SWAP_XY`
- `ADD_A_TO_X`
- `ADD_A_TO_Y`
- `ROR_A(k)`
- `XOR_A_Y`

Each one is individually reversible.

### 2. Parsing the binary as macro blocks

Instead of emulating the entire 6502 instruction stream at the instruction level, I matched the binary against byte patterns for those known macros.

For example:

- `ADD_CONST(c)` matches:

```text
08 18 69 ?? 28
```

- `XOR_CONST(c)` matches:

```text
08 49 ?? 28
```

- `ADD_A_TO_X` matches the loop:

```text
08 48 e8 38 e9 01 d0 fa 68 28
```

- `ROR_A(k)` matches a larger block containing:

```text
A2 ??
4A
90 02
09 80
CA
D0 F8
```

where the immediate loaded into `X` is the rotation count encoded by the macro.

Once the whole binary is parsed into a list of macro operations, the giant program becomes conceptually simple. It looks like this:

```text
ADD_CONST(0xa3)
SWAP_AX
ADD_A_TO_X
ROR_A(0xfb)
SWAP_AY
XOR_A_Y
XOR_CONST(0x0b)
SWAP_XY
ADD_A_TO_Y
...
```

This is much easier to reason about than millions of raw bytes.

### 3. Inverting the transformation

Because each macro is reversible, we can recover the original input by iterating over the macro list **backwards**.

Inverse rules:

- `ADD_CONST(c)` → `A = (A - c) mod 256`
- `XOR_CONST(c)` → same operation again, because XOR is self-inverse
- `SWAP_*` → same operation again
- `ADD_A_TO_X` → `X = (X - A) mod 256`
- `ADD_A_TO_Y` → `Y = (Y - A) mod 256`
- `ROR_A(k)` → rotate `A` left by `k`
- `XOR_A_Y` → `A ^= Y`

That gives us a direct inverse from final `(A, X, Y)` back to original `(A0, X0, Y0)`.

### 4. Why brute force is unnecessary

A naive approach would be to brute force all possible `2^24` inputs for each round, which is possible in theory but unnecessary and inefficient. The binary is deliberately structured so that reverse engineering the transformation is the intended path.

Once we reduce it to macro operations, each inversion is instant.

## Proof-of-Concept

The following solver reads `code.bin`, compresses it into macro operations, then accepts challenge output lines like:

```text
Final output #1: A=12 X=34 Y=56
```

and reconstructs the original inputs to submit back to the checker.

```python
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
        if pos == len(buf) - 1 and buf[pos] == 0x60:
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
    blob = Path("code.bin").read_bytes()
    ops = parse_macros(blob)

    print("Paste the challenge output, then Ctrl-D:")
    text = __import__("sys").stdin.read()
    triples = solve_lines(text, ops)
    for i, (a, x, y) in enumerate(triples, 1):
        print(f"Input #{i}: {a},{x},{y}")


if __name__ == "__main__":
    main()
```

### Usage

```bash
cp code-10k.bin code.bin
python3 solve_potato.py < outputs.txt
```

Where `outputs.txt` contains the 20 lines printed by the real challenge.

The script prints the original 20 `(A0, X0, Y0)` triples. Those can be copied directly back into the program to pass all checks and obtain the flag.

## P/S

- The clever part of this challenge is that the binary looks intimidating because of its size, but its logic is built from a very small vocabulary of reversible instruction templates.
- This is a good example of why pattern recognition can be more useful than raw emulation when reversing intentionally bloated binaries.
- The actual final flag is not present in the provided files, so it cannot be reproduced offline from the attachments alone. The solve script reconstructs the required answers, and the challenge service then reveals the flag in the format `bctf{...}`.
