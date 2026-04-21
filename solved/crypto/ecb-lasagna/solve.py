import base64
import itertools
from collections import defaultdict
from Crypto.Cipher import AES

y = base64.b64decode(open("output.txt").read().strip())
N = len(y) // 2

ye = [y[2 * k] for k in range(N)]
yo = [y[2 * k + 1] for k in range(N)]

A = list(range(32, 127))
cipher = AES.new(b"lasagna!" * 2, AES.MODE_ECB)
E = {ch: cipher.encrypt(bytes([ch]) * 16) for ch in A}

O = [{ch: E[ch][2 * r] ^ E[ch][2 * r + 1] for ch in A} for r in range(8)]

Acoef = [None] * 9
Acoef[0] = {ch: E[ch][0] for ch in A}
for r in range(1, 8):
    Acoef[r] = {ch: E[ch][2 * r - 1] ^ E[ch][2 * r] for ch in A}
Acoef[8] = {ch: E[ch][15] for ch in A}

invO0 = defaultdict(list)
for ch in A:
    invO0[O[0][ch]].append(ch)


def next_chars(state, k):
    need = yo[k]
    for r in range(1, 8):
        need ^= O[r][state[-r]]

    res = []
    for ch in invO0.get(need, []):
        acc = Acoef[0][ch]
        for r in range(1, 8):
            acc ^= Acoef[r][state[-r]]
        acc ^= Acoef[8][state[0]]
        if acc == ye[k]:
            res.append(ch)
    return res


prefix = tuple(b"bctf{")
states = set(prefix + tail for tail in itertools.product(A, repeat=3))
parents = {}

for k in range(8, N):
    nxt = set()
    p = {}
    for st in states:
        for ch in next_chars(st, k):
            ns = st[1:] + (ch,)
            if ns not in nxt:
                nxt.add(ns)
                p[ns] = st
    parents[k] = p
    states = nxt

assert len(states) == 1
state = next(iter(states))
state_by_k = {N - 1: state}

for k in range(N - 1, 8, -1):
    state_by_k[k - 1] = parents[k][state_by_k[k]]

init_state = parents[8][state_by_k[8]]

m = list(init_state)
for k in range(8, N):
    m.append(state_by_k[k][-1])

flag = bytes(m).decode()
print(flag)

# bctf{y0u'v3_h349d_0f_5p4gh3tt1_c0d3,_8ut_d1d_y0u_kn0w_l454gn4_c0d3_4150_3x15t5?_1t_m34n5_y0u_c4n't_m4k3_4_ch4ng3_50m3wh3r3_w1th0ut_m4k1n6_4_ch4ng3_1n_m4n7_0th3r_p4rt5_0f_th3_c0d5.}
