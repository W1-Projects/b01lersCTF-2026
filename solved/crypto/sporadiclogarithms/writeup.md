# sporadiclogarithms

**Category:** Crypto

## Description
> Pass 5 rounds. In each round, find any `x` in `[0, 262144]` such that `h = s_{g,phi}(x)`, where `phi(a) = c*a*c^-1`, over `GL(3,65537)`.
```
#!/usr/bin/env python3

from __future__ import annotations

import os
import random
import sys
from dataclasses import dataclass
from typing import Dict, Tuple

from sage.all import GF, MatrixSpace, divisors, identity_matrix


def eprint(msg: str) -> None:
    print(msg, flush=True)


def mat_key(m) -> Tuple[int, ...]:
    return tuple(int(x) for x in m.list())


@dataclass
class Params:
    p: int = int(os.getenv("SAGE_CHALLENGE_P", "65537"))
    n: int = int(os.getenv("SAGE_CHALLENGE_N", "3"))
    rounds: int = int(os.getenv("SAGE_CHALLENGE_ROUNDS", "5"))
    bound: int = int(os.getenv("SAGE_CHALLENGE_BOUND", "262144"))
    max_queries: int = int(os.getenv("SAGE_CHALLENGE_MAX_QUERIES", "10000"))
    max_c_order: int = int(os.getenv("SAGE_CHALLENGE_MAX_C_ORDER", "8"))


class SageBlackBox:
    def __init__(self, M, c) -> None:
        self.M = M
        self.c = c
        self.c_inv = c.inverse()
        self.table = []
        self.index: Dict[Tuple[int, ...], int] = {}

        self.id_elem = identity_matrix(M.base_ring(), M.nrows())
        self.id = self._add_elem(self.id_elem)

    def _add_elem(self, x) -> int:
        k = mat_key(x)
        if k in self.index:
            return self.index[k]
        self.table.append(x)
        h = len(self.table)
        self.index[k] = h
        return h

    def rand(self) -> int:
        while True:
            x = self.M.random_element()
            if x.det() != 0:
                return self._add_elem(x)

    def mul(self, a: int, b: int) -> int:
        return self._add_elem(self.table[a - 1] * self.table[b - 1])

    def inv(self, a: int) -> int:
        return self._add_elem(self.table[a - 1].inverse())

    def eq(self, a: int, b: int) -> bool:
        return self.table[a - 1] == self.table[b - 1]

    def ord(self, a: int) -> int:
        return int(self.table[a - 1].multiplicative_order())

    def phi(self, a: int) -> int:
        x = self.table[a - 1]
        return self._add_elem(self.c * x * self.c_inv)


def choose_small_order_conjugator(F, n: int, max_order: int):
    divs = [d for d in divisors(F.order() - 1) if 1 < int(d) <= max_order]
    if not divs:
        raise RuntimeError("no suitable small order available; increase field size")

    m = int(random.choice(divs))
    g = F.multiplicative_generator()
    d = g ** ((F.order() - 1) // m)

    c = identity_matrix(F, n)
    c[0, 0] = d
    return c, m


def hol_mul(x, y):
    a, c1 = x
    b, c2 = y
    return a * (c1 * b * c1.inverse()), c1 * c2


def hol_pow(base, e, F, n: int):
    one = identity_matrix(F, n)
    result = (one, one)
    cur = base
    k = e
    while k > 0:
        if k & 1:
            result = hol_mul(result, cur)
        cur = hol_mul(cur, cur)
        k >>= 1
    return result


def print_help() -> None:
    eprint("Commands:")
    eprint("  help")
    eprint("  handles               # print one,g,c,h handles")
    eprint("  mul <a> <b>           # return handle of a*b")
    eprint("  inv <a>               # return handle of a^-1")
    eprint("  phi <a>               # return handle of c*a*c^-1")
    eprint("  eq <a> <b>            # return 1 if equal else 0")
    eprint("  submit <x>            # submit exponent x")
    eprint("  quit")


def parse_nonneg_int(tok: str):
    if not tok.isdigit():
        return None
    return int(tok)


def run_round(params: Params, ridx: int) -> bool:
    F = GF(params.p)
    M = MatrixSpace(F, params.n)

    eprint(f"\n=== Round {ridx}/{params.rounds} setup ===")
    eprint("[setup] choosing conjugator c with small order...")
    c, c_order = choose_small_order_conjugator(F, params.n, params.max_c_order)

    bb = SageBlackBox(M, c)
    eprint(f"[setup] c order={c_order}")

    g = bb.rand()
    g_ord = bb.ord(g)
    eprint(f"[setup] sampled g with ord={g_ord}")

    bound = params.bound
    x_secret = random.randint(0, bound)

    g_elem = bb.table[g - 1]
    h_hol = hol_pow((g_elem, c), x_secret, F, params.n)
    h = bb._add_elem(h_hol[0])
    c_handle = bb._add_elem(c)

    eprint(f"Find any x in [0, {bound}] such that h = s_{{g,phi}}(x),")
    eprint("where phi(a) = c*a*c^-1 and s_{g,phi}(x)")
    eprint(f"group=GL({params.n},{params.p}) one={bb.id} g={g} c={c_handle} h={h}")
    print_help()

    queries = 0
    while True:
        print("bb> ", end="", flush=True)
        line = sys.stdin.readline()
        if line == "":
            eprint("EOF")
            return False
        toks = line.strip().split()
        if not toks:
            continue

        cmd = toks[0].lower()

        if cmd == "help":
            print_help()
            continue

        if cmd == "handles":
            eprint(f"one={bb.id} g={g} c={c_handle} h={h}")
            continue

        if cmd == "submit":
            if len(toks) != 2:
                eprint("usage: submit <x>")
                continue
            x = parse_nonneg_int(toks[1])
            if x is None:
                eprint("error: x must be a non-negative integer")
                continue

            hx = hol_pow((g_elem, c), x, F, params.n)[0]
            if hx == bb.table[h - 1]:
                eprint("correct")
                return True
            eprint("wrong")
            return False

        if cmd == "quit":
            return False

        if cmd in ("mul", "inv", "phi", "eq"):
            if queries >= params.max_queries:
                eprint("query limit exceeded")
                return False
            queries += 1

        if cmd == "mul":
            if len(toks) != 3:
                eprint("usage: mul <a> <b>")
                continue
            a = parse_nonneg_int(toks[1])
            b = parse_nonneg_int(toks[2])
            if a is None or b is None:
                eprint("error: handles must be non-negative integers")
                continue
            if a < 1 or b < 1 or a > len(bb.table) or b > len(bb.table):
                eprint("error: unknown handle")
                continue
            eprint(str(bb.mul(a, b)))
            continue

        if cmd == "inv":
            if len(toks) != 2:
                eprint("usage: inv <a>")
                continue
            a = parse_nonneg_int(toks[1])
            if a is None:
                eprint("error: handle must be a non-negative integer")
                continue
            if a < 1 or a > len(bb.table):
                eprint("error: unknown handle")
                continue
            eprint(str(bb.inv(a)))
            continue

        if cmd == "phi":
            if len(toks) != 2:
                eprint("usage: phi <a>")
                continue
            a = parse_nonneg_int(toks[1])
            if a is None:
                eprint("error: handle must be a non-negative integer")
                continue
            if a < 1 or a > len(bb.table):
                eprint("error: unknown handle")
                continue
            eprint(str(bb.phi(a)))
            continue

        if cmd == "eq":
            if len(toks) != 3:
                eprint("usage: eq <a> <b>")
                continue
            a = parse_nonneg_int(toks[1])
            b = parse_nonneg_int(toks[2])
            if a is None or b is None:
                eprint("error: handles must be non-negative integers")
                continue
            if a < 1 or b < 1 or a > len(bb.table) or b > len(bb.table):
                eprint("error: unknown handle")
                continue
            eprint("1" if bb.eq(a, b) else "0")
            continue

        eprint("unknown command (type help)")


def main() -> int:
    params = Params()
    flag = os.getenv("FLAG", "flag{set_FLAG_env}")

    eprint(f"Pass {params.rounds} rounds to get the flag.")
    for i in range(1, params.rounds + 1):
        if not run_round(params, i):
            eprint("Challenge failed.")
            return 1

    eprint(flag)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

```

## Overview
The challenge is a twisted discrete-log problem implemented as powers of `(g, c)` in a semidirect product, but `c` is intentionally chosen with tiny order `m <= 8`. That makes `phi^m = id`, so the non-commutative prefix product `s_{g,phi}(x)` splits into a small residue part and an ordinary discrete log on a block element. The solve first recovers `x mod m` from `h^{-1} g phi(h)`, then solves the remaining exponent with baby-step giant-step and submits `x = qm + r`.

## Technical details
The core weakness is in `choose_small_order_conjugator()`: the automorphism is not generic at all, because the challenge explicitly samples a conjugator `c` of very small multiplicative order.

```python
def choose_small_order_conjugator(F, n: int, max_order: int):
    divs = [d for d in divisors(F.order() - 1) if 1 < int(d) <= max_order]
    m = int(random.choice(divs))
    g = F.multiplicative_generator()
    d = g ** ((F.order() - 1) // m)
    c = identity_matrix(F, n)
    c[0, 0] = d
    return c, m
```

With the default parameters, `p = 65537`, so `p - 1 = 65536` and the only allowed orders are `m in {2, 4, 8}`. Since `phi(a) = c a c^{-1}`, this immediately gives `phi^m = id`.

The service computes the secret target from `hol_pow((g_elem, c), x_secret, ...)`, where multiplication is:

```python
def hol_mul(x, y):
    a, c1 = x
    b, c2 = y
    return a * (c1 * b * c1.inverse()), c1 * c2
```

Therefore

`(g, c)^x = (g * phi(g) * phi^2(g) * ... * phi^{x-1}(g), c^x)`,

so the published handle `h` is exactly the first component

`s_{g,phi}(x) = prod_{i=0}^{x-1} phi^i(g)`.

Because `phi` has tiny order, this product is periodic. Let

- `terms[i] = phi^i(g)` for `0 <= i < m`,
- `prefix[r] = prod_{i=0}^{r-1} terms[i]`,
- `z = prefix[m] = s_{g,phi}(m)`.

Write `x = qm + r` with `0 <= r < m`. Since `phi^m = id`, one block of length `m` always contributes the same first component `z`, and `(g, c)^m = (z, 1)`. Hence

`h = s_{g,phi}(x) = z^q * prefix[r]`.

This is the exact reduction exploited by `solve.py`: the original twisted problem collapses to a standard discrete log in the subgroup generated by `z`, once `r` is known.

The residue `r = x mod m` is recovered from a short algebraic identity. If `h = s_{g,phi}(x)`, then

`h^{-1} * g * phi(h) = phi^x(g)`.

Expanding the products shows all intermediate terms cancel:

`h^{-1} g phi(h) = (phi^{x-1}(g)^{-1} ... phi(g)^{-1} g^{-1}) g (phi(g) ... phi^x(g)) = phi^x(g)`.

Since `phi^m = id`, this equals `phi^r(g)`. The solver queries the orbit `[g, phi(g), ..., phi^{m-1}(g)]` and matches the witness against it:

```python
inv_h = self.inv(h)
phi_h = self.command_int(f"phi {h}")
tmp = self.command_int(f"mul {inv_h} {g}")
witness = self.command_int(f"mul {tmp} {phi_h}")
residues = [r for r, term in enumerate(terms) if term == witness]
```

The output of this step is a small candidate set for `r`. Multiple residues are possible only if the orbit of `g` under `phi` has length smaller than `m`; `solve.py` handles that by trying every matching residue.

Once a candidate `r` is fixed, the remaining unknown is `q`:

`target = h * prefix[r]^{-1} = z^q`, with `0 <= q <= floor((bound - r) / m)`.

That is an ordinary discrete log, and the bound is tiny enough for baby-step giant-step. With `bound = 262144` and `m >= 2`, the worst case is `q <= 131072`, so `sqrt(q)` is at most `363`. This fits easily under the per-round query cap of `10000`.

One implementation detail makes the remote attack even cleaner: the black-box is not truly opaque. `_add_elem()` interns each matrix and reuses the same handle for repeated values:

```python
def _add_elem(self, x) -> int:
    k = mat_key(x)
    if k in self.index:
        return self.index[k]
```

So the handle itself is a canonical identifier for a group element. `solve.py` uses those integer handles directly as hash-table keys in BSGS, which avoids spending queries on explicit `eq` checks and enables the batched `mul_chain()` optimization.

## Proof-of-Concept
- Step 1: Parse one round and enumerate the short `phi`-orbit of `g`.

```python
m, one, g, _c, h = self.parse_round(banner)
terms = [g]
for _ in range(1, m):
    terms.append(self.command_int(f"phi {terms[-1]}"))
```

This produces `[g, phi(g), ..., phi^{m-1}(g)]`. Because `m` is the order of `c`, this list contains every possible value of `phi^x(g)` modulo `m`.

- Step 2: Recover the residue class `r = x mod m`.

```python
inv_h = self.inv(h)
phi_h = self.command_int(f"phi {h}")
tmp = self.command_int(f"mul {inv_h} {g}")
witness = self.command_int(f"mul {tmp} {phi_h}")
residues = [r for r, term in enumerate(terms) if term == witness]
```

The artifact here is `witness = phi^x(g) = phi^r(g)`, so `residues` is the small set of valid values for `r`.

- Step 3: Remove the residue prefix and reduce the problem to `z^q`.

```python
prefix = [one]
cur = one
for i in range(m):
    cur = self.command_int(f"mul {cur} {terms[i]}")
    prefix.append(cur)

block = prefix[m]
target = self.command_int(f"mul {h} {self.inv(prefix[r])}")
```

This yields `block = z = s_{g,phi}(m)` and `target = z^q`. At this point the twisted product is gone; only a bounded classical DLP remains.

- Step 4: Solve the bounded DLP with baby-step giant-step and reconstruct `x`.

```python
q = self.solve_dlog(block, target, (self.bound - r) // m)
answer = q * m + r
self.tube.sendline(f"submit {answer}")
```

The artifact is a valid exponent `x` satisfying the challenge equation. Running `solve.py` against the live service solved all five rounds and returned the flag.

## P/S
`vd.py` implements the same cryptanalytic reduction in a less optimized form and even recomputes `ord(c)` by repeated multiplication, so the attack does not depend on the banner printing `c order=...`. The batching logic in `solve.py` is only a transport optimization; the actual break is the small-order inner automorphism, which turns the twisted logarithm into a residue recovery plus one ordinary BSGS instance.
