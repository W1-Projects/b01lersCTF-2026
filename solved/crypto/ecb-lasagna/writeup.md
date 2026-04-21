# ECB Lasagna

**Category:** Crypto

## Description

> The organizer's original description was not included in the local folder. Based on the provided artifacts, the task is to recover the real flag from `dist/chall.py` and the Base64 ciphertext in `dist/output.txt`.

## Overview

This challenge is not about breaking AES. The real bug is the homemade construction built around AES-ECB in [dist/chall.py](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/chall.py).

The encryption script does four important things:

1. It reads the flag.
2. It duplicates every character, so `abc` becomes `aabbcc`.
3. For each position `i`, it encrypts the 16-byte block `flag[i] * 16` with a fixed AES key.
4. It shifts that 16-byte ciphertext by `i` bytes around the output buffer and XORs all layers together.

Because AES-ECB is deterministic, the block produced by a character `c` is always the same:

`E(c) = AES_k(c * 16)`

So instead of thinking about the ciphertext as "360 random bytes", we can think of it as many overlapped copies of only a small set of possible 16-byte blocks, one block per printable character.

The character-duplication step is what makes the challenge solvable. Since every original flag byte appears twice in a row before encryption, neighboring output bytes are no longer independent. After splitting the final ciphertext into even and odd positions, each equation depends on only a small sliding window of recent plaintext characters. That is exactly the structure the solver exploits.

Another useful observation comes from [dist/flag.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/flag.txt). It contains only `bctf{fake_flag}`, which has length 15. After duplication, that would produce only 30 output bytes, but [dist/output.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/output.txt) decodes to 360 bytes. So `dist/flag.txt` is definitely a placeholder/decoy and not the flag that generated the provided ciphertext.

## Technical details

Let the real flag be:

`m[0], m[1], ..., m[n-1]`

After the duplication step in `chall.py`, we get a new string `f` of length `2n`:

`f[2k] = f[2k+1] = m[k]`

Now define:

`E(c) = AES_k(c * 16)`

where the key is the fixed value `b"lasagna!" * 2`.

For each index `i`, the script computes the 16-byte block `E(f[i])`, shifts it by `i` bytes, wraps the overflow back to the front, and XORs it into the result buffer. So if the final output is `y`, then each output byte is:

`y[j] = XOR_{t=0}^{15} E(f[(j - t) mod 2n])[t]`

The provided Base64 string in [dist/output.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/output.txt) decodes to 360 bytes. Since the duplicated plaintext has length `2n`, we get:

`2n = 360  =>  n = 180`

So the real flag is 180 characters long.

The next trick is to separate the ciphertext into even and odd positions:

- `ye[k] = y[2k]`
- `yo[k] = y[2k+1]`

Because `f[2k]` and `f[2k+1]` are the same character, the terms inside these equations naturally pair up.

For odd positions:

`yo[k] = O0(m[k]) ^ O1(m[k-1]) ^ ... ^ O7(m[k-7])`

where:

`Or(c) = E(c)[2r] ^ E(c)[2r+1]`

This happens because for `y[2k+1]`, the bytes `E(c)[2r]` and `E(c)[2r+1]` both come from the same duplicated flag character.

For even positions, the pattern is almost the same, except the first and last terms are unpaired:

`ye[k] = A0(m[k]) ^ A1(m[k-1]) ^ ... ^ A7(m[k-7]) ^ A8(m[k-8])`

with:

- `A0(c) = E(c)[0]`
- `Ar(c) = E(c)[2r-1] ^ E(c)[2r]` for `1 <= r <= 7`
- `A8(c) = E(c)[15]`

This is exactly what [solve.py](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/solve.py) precomputes.

From there, the solver treats the problem as a sliding-window search:

1. Assume the flag uses printable ASCII (`32..126`).
2. Use the known flag prefix `bctf{`.
3. Brute-force only the next 3 printable characters, so we get every possible 8-byte initial window `m[0..7]`.
4. For each later position `k`, use `yo[k]` to predict which next characters are possible.
5. Use `ye[k]` as a second check to reject wrong branches immediately.

Why does this work well? Because once we know the previous 8 plaintext bytes, the next equations only depend on that 8-byte window plus one unknown new character. That means each step is heavily constrained, and almost all candidates die very quickly.

The initial brute-force is only:

`95^3 = 857375`

which is completely manageable. After that, the search becomes a DP/BFS over states of length 8. In practice, the candidate set collapses quickly, and the solver ends with exactly one valid plaintext.

## Proof-of-Concept

The included solver works after placing [dist/output.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/output.txt) next to [solve.py](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/solve.py) as `output.txt`:

```bash
$ python3 solve.py
bctf{y0u'v3_h349d_0f_5p4gh3tt1_c0d3,_8ut_d1d_y0u_kn0w_l454gn4_c0d3_4150_3x15t5?_1t_m34n5_y0u_c4n't_m4k3_4_ch4ng3_50m3wh3r3_w1th0ut_m4k1n6_4_ch4ng3_1n_m4n7_0th3r_p4rt5_0f_th3_c0d5.}
```

I also verified the result by re-running the encryption logic from [dist/chall.py](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/chall.py) on the recovered plaintext. The regenerated Base64 ciphertext matches [dist/output.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/output.txt) exactly.

## P/S

- AES-ECB itself is not being broken here; the weakness is the custom "duplicate, shift, then XOR" construction built on top of deterministic ECB blocks.
- The duplication step is the key mistake because it creates pairwise relationships between adjacent ciphertext positions, letting us reduce the problem to local equations on a short sliding window.
- [dist/flag.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/flag.txt) is a decoy. Its length is incompatible with the 360-byte ciphertext in [dist/output.txt](/home/team/CTF%20Cryptography/bo1lersCTF2026/ecb-lasagna/dist/output.txt).
- Re-encrypting the recovered plaintext reproduces the exact provided ciphertext, confirming that the recovered flag is correct.
