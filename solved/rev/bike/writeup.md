# bike

**Category:** Rev

## Description

> rev/bike  
> beginner  
> CygnusX  
> Get your bike down from the tree!!
>
> Format: `bctf{...}`

## Overview

This challenge is a reverse engineering task built around a custom input checker. The binary does not compare the user input directly against the flag. Instead, it builds a weighted binary tree from a static table, interprets the input as a bitstream, decodes that bitstream through the tree, and compares the decoded bytes against a fixed byte array in the binary.

The clean solve is to reverse that process: recover the tree construction logic, rebuild the exact tree from the embedded table, derive the bit-path for every leaf, and then re-encode the expected byte array back into a valid input string. Doing that produces the intended flag:

`bctf{now_you_can_bike_around:)}`

## Technical details

Basic triage with `file`, `strings`, and `objdump` shows that the ELF is not stripped and contains useful symbols such as `main`, `build_tree`, `freqs`, and `expected`.

The logic in `main` is:

- call `setup()`
- print `flag? `
- call `build_tree(freqs, 0, 0x7f)`
- read up to 31 bytes with `fgets`
- process the input one bit at a time, from most significant bit to least significant bit
- walk left on bit `0`, right on bit `1`
- whenever a leaf is reached, emit its byte and reset back to the root
- compare the decoded output against the static `expected` array

The tree itself is built recursively from 128 entries in `freqs`. Each entry stores a symbol and a `float` weight. The recursive split tries to divide a contiguous range into two nearly balanced halves by cumulative weight, which is effectively a Shannon-Fano-style construction.

Recovered node layout:

```c
struct Node {
    uint8_t value;
    float weight;
    struct Node *left;
    struct Node *right;
};
```

Recovered `build_tree` logic:

```c
Node *build_tree(Entry *freqs, int lo, int hi) {
    if (lo == hi) {
        Node *n = malloc(0x18);
        n->value = freqs[lo].symbol;
        n->weight = freqs[lo].weight;
        n->left = NULL;
        n->right = NULL;
        return n;
    }

    float total = 0.0f;
    for (int i = lo; i < hi; i++) {
        total += freqs[i].weight;
    }

    float acc = 0.0f;
    int split = lo;
    for (int i = lo; i <= hi; i++) {
        if (acc + freqs[i].weight > total / 2.0f)
            break;
        acc += freqs[i].weight;
        split = i;
    }

    Node *n = malloc(0x18);
    n->value = 0;
    n->weight = total;
    n->left = build_tree(freqs, lo, split);
    n->right = build_tree(freqs, split + 1, hi);
    return n;
}
```

A detail that matters when reproducing the tree is that the arithmetic behaves like `float32`, not full Python double precision. If the script does not preserve 32-bit float rounding, one of the recursive splits can differ and the recovered input will be wrong.

The checker also has a bug: it only compares the produced decoded bytes against the start of `expected`, and it never checks that the decoded output length matches the full expected length. That means multiple candidates can pass locally. Even so, only one candidate cleanly matches the challenge flag format.

## Proof-of-Concept

The following script parses the embedded data directly from the binary, rebuilds the exact tree, derives the code for every symbol, re-encodes the `expected` array, and prints valid candidate inputs. The intended result is `bctf{now_you_can_bike_around:)}`.

```python
#!/usr/bin/env python3
import struct
from pathlib import Path

BIN = "bike"

# Offsets for this binary
FREQS_OFF = 0x3020
EXPECTED_OFF = 0x3420
NUM_FREQS = 128
EXPECTED_LEN = 35

def f32(x):
    return struct.unpack("<f", struct.pack("<f", float(x)))[0]

class Node:
    __slots__ = ("value", "weight", "left", "right")
    def __init__(self, value, weight, left=None, right=None):
        self.value = value
        self.weight = weight
        self.left = left
        self.right = right

def parse_freqs(blob):
    freqs = []
    for i in range(NUM_FREQS):
        off = FREQS_OFF + i * 8
        sym = blob[off]
        weight = struct.unpack("<f", blob[off + 4:off + 8])[0]
        freqs.append((sym, weight))
    return freqs

def build_tree(freqs, lo, hi):
    if lo == hi:
        sym, w = freqs[lo]
        return Node(sym, w)

    total = f32(0.0)
    i = lo
    while i < hi:
        total = f32(total + freqs[i][1])
        i += 1

    acc = f32(0.0)
    split = lo
    half = f32(total / f32(2.0))

    i = lo
    while i <= hi:
        nxt = f32(acc + freqs[i][1])
        if nxt > half:
            break
        acc = nxt
        split = i
        i += 1

    return Node(
        0,
        total,
        build_tree(freqs, lo, split),
        build_tree(freqs, split + 1, hi),
    )

def collect_codes(node, prefix="", out=None):
    if out is None:
        out = {}
    if node.left is None and node.right is None:
        out[node.value] = prefix
        return out
    collect_codes(node.left, prefix + "0", out)
    collect_codes(node.right, prefix + "1", out)
    return out

def decode(root, data):
    cur = root
    out = bytearray()
    for b in data:
        for i in range(8):
            if cur.left is None and cur.right is None:
                out.append(cur.value)
                cur = root
            bit = (b >> (7 - i)) & 1
            cur = cur.right if bit else cur.left
    return bytes(out)

def bits_to_bytes(bits):
    assert len(bits) % 8 == 0
    return bytes(int(bits[i:i+8], 2) for i in range(0, len(bits), 8))

def main():
    blob = Path(BIN).read_bytes()
    freqs = parse_freqs(blob)
    expected = blob[EXPECTED_OFF:EXPECTED_OFF + EXPECTED_LEN]

    root = build_tree(freqs, 0, 127)
    codes = collect_codes(root)

    bitstream = "".join(codes[b] for b in expected)

    # Pad to a full number of bytes so the last symbol is emitted
    for tail in ("00", "01", "10", "11"):
        candidate = bits_to_bytes(bitstream + tail)
        decoded = decode(root, candidate)
        if decoded == expected:
            print(repr(candidate))

if __name__ == "__main__":
    main()
```

Example output includes several locally accepted candidates because of the checker bug, but the only one that matches the expected flag format is:

```text
b'bctf{now_you_can_bike_around:)}'
```

Verification:

```bash
printf 'bctf{now_you_can_bike_around:)}' | ./bike
```

Expected output:

```text
flag? Congrats on getting your bike down!
```

## P/S

- Main tools used: `file`, `strings`, `objdump`, and Python 3.
- The challenge is beginner-friendly because the binary preserves symbols and the decoding logic is recoverable statically.
- The most important reversing detail is reproducing the exact tree-building behavior, especially the `float32` arithmetic.
- The intended flag is:

`bctf{now_you_can_bike_around:)}`

