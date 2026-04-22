#!/usr/bin/env python3
import struct
from pathlib import Path

BIN = "bike"
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
    return bytes(int(bits[i:i + 8], 2) for i in range(0, len(bits), 8))


def main():
    blob = Path(BIN).read_bytes()
    freqs = parse_freqs(blob)
    expected = blob[EXPECTED_OFF:EXPECTED_OFF + EXPECTED_LEN]

    root = build_tree(freqs, 0, 127)
    codes = collect_codes(root)

    bitstream = "".join(codes[b] for b in expected)

    candidates = []
    for tail in ("00", "01", "10", "11"):
        candidate = bits_to_bytes(bitstream + tail)
        if decode(root, candidate) == expected:
            candidates.append(candidate)

    for c in candidates:
        if c.startswith(b"bctf{") and c.endswith(b"}"):
            print(c.decode())
            return

    print("No flag-shaped candidate found")


if __name__ == "__main__":
    main()