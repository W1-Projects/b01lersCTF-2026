# Yuri

**Category:** Forensics

## Description

> The organizer's original description was not included in the local archive. From the provided files, the task is to inspect `yuri.txt` and `yuri_1.txt` and recover the hidden flag.

## Overview

After extracting [yuri.tar.gz](/home/team/CTF%20Cryptography/bo1lersCTF2026/yuri.tar.gz), the archive contains two UTF-8 text files: [yuri.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/yuri/yuri.txt) and [yuri_1.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/yuri/yuri_1.txt).

At first glance they look almost identical, but [yuri_1.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/yuri/yuri_1.txt) contains a small number of strange substituted characters such as `Æ`, `È`, `ß`, `Ñ`, and `₈`. That strongly suggests the flag is hidden in the differences between the two texts rather than in the story content itself.

## Technical details

Comparing the two files character-by-character shows exactly 30 modified positions.

Examples:

- `d` became `Æ`
- `e` became `È`
- `t` became `è`
- `l` became `Ò`
- `A` became `¼`

The key observation is that the Unicode codepoint difference at each changed position is printable ASCII:

- `ord('Æ') - ord('d') = 198 - 100 = 98 = 'b'`
- `ord('È') - ord('e') = 200 - 101 = 99 = 'c'`
- `ord('è') - ord('t') = 232 - 116 = 116 = 't'`
- `ord('Ò') - ord('l') = 210 - 108 = 102 = 'f'`
- `ord('¼') - ord('A') = 188 - 65 = 123 = '{'`

Doing this for all 30 changed characters reconstructs the full flag.

In other words, if `a` is a character from [yuri.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/yuri/yuri.txt) and `b` is the corresponding character from [yuri_1.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/yuri/yuri_1.txt), then every modified location encodes one flag byte as:

`flag_char = chr(ord(b) - ord(a))`

## Proof-of-Concept

This short script extracts the flag directly:

```python
from pathlib import Path

a = Path("yuri.txt").read_text(encoding="utf-8")
b = Path("yuri_1.txt").read_text(encoding="utf-8")

flag = "".join(
    chr(ord(cb) - ord(ca))
    for ca, cb in zip(a, b)
    if ca != cb
)

print(flag)
```

Running it prints:

```text
bctf{w3_l0ve_yur1_rB4DN8aULH9}
```

## P/S

- The hidden data is not stored in added lines or obvious markers; it is encoded in the numeric gap between matching characters in the two files.
- A plain visual read is enough to notice corruption, but a character-by-character diff is what turns that clue into the flag.
- Recovered flag: `bctf{w3_l0ve_yur1_rB4DN8aULH9}`
