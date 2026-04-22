# piano

**Category:** Rev

## Description
> A sky full of stars

## Overview
This challenge is a ptrace-driven virtual machine hidden inside a stripped static ELF. The child process appears to execute a forest of bizarre memory-access instructions through `r15`, but those accesses are intentionally aimed at a zero-length memfd-backed mapping so they fault with `SIGBUS`. The parent process traces the child, inspects the faulting instruction bytes, and interprets each fault as a semantic VM action.

The most important conceptual shift is that the binary is not meant to be read as ordinary x86. The faulting instructions are only an encoded instruction stream. The real semantics are assigned by the ptracing parent, and those semantics turn out to be extremely simple:

- values are represented as unary linked-list lengths,
- each interesting fault means either “push” or “pop”,
- helper functions compose those operations into addition, subtraction, and multiplication,
- underflow conditions become linear inequalities,
- terminal checks become linear equalities.

So despite the anti-debugging flavor, the whole checker collapses into a linear system over the 36 input bytes. Solving that system recovers the flag directly:

```text
bctf{b#fqQZ*OEQAZKTDAYyl6VPRomVPRom}
```

## Technical details
The challenge binary is:

```text
ELF 64-bit LSB executable, x86-64, statically linked, stripped
```

The visible strings already reveal the user-facing behavior:

```text
Correct!
Wrong
Usage: <flag>
```

and the argument checks in `main` enforce:

- `argc == 2`
- `strlen(argv[1]) == 0x24`

So the checker expects exactly 36 input bytes.

### The real setup happens before `main`
The key initialization happens in the routine at `0x401030`. In effect, that routine:

1. creates an anonymous mapping,
2. creates a `memfd`,
3. mmaps `0x1000` bytes from that `memfd`,
4. closes the fd,
5. stores the resulting mapping in `r15`,
6. sets `r14 = -1`.

The subtle but crucial detail is that the memfd-backed file mapping is created without extending the file size first. So reads through that mapping do not behave like normal loads. Instructions such as:

```asm
movzx  r12w, BYTE PTR [r15+0x36]
movsx  rdi, WORD PTR [r15+0x6c]
xor    bp, WORD PTR [r15+0x27]
```

raise `SIGBUS` instead.

That means the huge number of odd `[r15+...]` instructions are not there for their architectural effect. They are VM opcodes encoded as faulting memory references.

The value `r14 = -1` is equally important. The list-manipulation helpers compare list heads against `r14`, so `-1` is the sentinel value representing an empty linked list.

### Parent/child architecture
`main` forks the computation using `clone3`.

On the child side:

- it calls `ptrace(PTRACE_TRACEME, ...)`,
- sends itself `SIGSTOP`,
- allocates a collection of list-head variables,
- initializes one list-based counter per input byte,
- then enters the generated checker code full of faulting `[r15+...]` instructions.

On the parent side, execution begins at `0x402888` and acts as the VM supervisor. Its main loop repeatedly:

1. waits for the child to stop,
2. examines the stop reason,
3. fetches the child register state with `PTRACE_GETREGS`,
4. reads the faulting instruction bytes with `PTRACE_PEEKTEXT`,
5. classifies the fault via the decoder helpers,
6. patches the child register state to emulate the intended VM action,
7. writes registers back with `PTRACE_SETREGS`,
8. resumes execution with `PTRACE_CONT`.

So the child “runs” nonsensical x86, but the parent is the real interpreter.

### The decoder’s important rule
The decoder path in the parent, especially around `0x402c00` and `0x402c60`, normalizes the faulting instruction bytes and then decides whether the current opcode means push or pop.

The practically important rule recovered from the notes is:

- the classifier ultimately inspects bit 2 of a selected instruction byte,
- that choice comes from the logic around `0x402c3a`,
- and the static model that matched the live checker was:

```python
def sign_rule(bs):
    return 1 if len(bs) > 1 and (bs[1] & 4) else -1
```

So the enormous faulting instruction stream reduces to a stream of `+1` and `-1` events.

### Primitive VM helpers
Once viewed semantically instead of architecturally, the core helpers are tiny.

#### `0x4036d0` — allocate an empty counter
This function allocates 8 bytes and stores the sentinel `r14` into it:

```text
new variable := 0
```

where “0” means an empty linked list.

#### `0x403770` — push
This helper allocates a new node, links it at the front of the list, and returns via `rt_sigreturn`. Semantically:

```text
x := x + 1
```

#### `0x4037a0` — pop
This helper removes one node from the front of the list and frees it. Semantically:

```text
x := x - 1
```

but only if the list is nonempty. So every pop also contributes a semantic side-condition that the current value must stay nonnegative.

### Input bytes are stored as unary counters
After initialization, the child allocates 36 counter heads and initializes them from the 36 input bytes. The helper `0x4036f0` loops `sil` times and triggers a single faulting instruction each iteration. The parent classifies that fault as push, so the helper simply builds a list of length `c`.

Therefore:

```text
call 0x4036f0(dst, c)  =>  dst := c
```

where `c` is the input byte value.

So the child state after initialization is:

```text
v0  = flag[0]
v1  = flag[1]
...
v35 = flag[35]
```

with each integer represented as a linked-list length.

### Arithmetic combinators: add and subtract
Two helpers expose the arithmetic structure directly.

#### `0x403730` — add source into destination
This helper walks the source list one node at a time, and for each node it triggers a fault classified as push on the destination:

```text
dst += src
```

#### `0x403750` — subtract source from destination
This helper has the same structure, but the repeated fault is classified as pop:

```text
dst -= src
```

with the additional requirement that the destination never underflows. So every use of this helper contributes a linear inequality like:

```text
dst >= src
```

### Two classes of generated helper functions
After the initial setup, `main` performs a long sequence of calls into generated helper functions. These fall into two useful categories.

#### Direct-stream helpers
A direct-stream helper is mostly a straight-line sequence of faulting instructions. Once each fault is mapped to `+1` or `-1`, the helper reduces to:

```text
x >= req
x := x + net
```

where:

- `net` is the total signed sum of the sequence,
- `req` is the minimum prefix budget needed to avoid underflow.

For a sequence like:

```text
+1, +1, -1, -1, -1, +1
```

we compute:

- `net = -1`
- prefix values = `1, 2, 1, 0, -1, 0`
- minimum prefix = `-1`
- therefore `req = 1`

So the helper means “the current variable must start at least 1, and ends 1 smaller than before”.

The notes report examples like:

```text
net -37, plus 296, minus 333
```

meaning a direct helper may execute hundreds of effective VM operations while still collapsing to one small linear update plus one prefix constraint.

#### Prologue helpers
The second class begins with a recognizable stack prologue like:

```asm
push   rbx
push   r12
push   rbp
mov    rbx, rdi
push   -1
mov    r12, rsp
mov    rdi, r12
```

This pattern means the helper is creating a temporary local accumulator, applying a fixed push/pop stream into that temporary accumulator once per element of the original variable, then replacing the old variable with the temporary one.

If the per-iteration local effect is `p`, and the original variable has value `n`, then after the loop the temporary accumulator has value:

```text
p * n
```

The final loop empties the original variable and replaces it with that temporary accumulator, so the helper semantically becomes:

```text
x := p * x
```

The notes recovered many such multipliers, for example:

```text
0x4a5370 -> mult 251
0x734180 -> mult 231
0x736b90 -> mult 183
0x737a10 -> mult 101
0x738dc0 -> mult 363
0x73dd80 -> mult 427
```

All recorded prologue helpers had zero internal underflow, so they really were exact positive multiplications.

That insight was the turning point of the solve: the earlier incomplete model treated only direct helpers correctly, but the checker did not line up until the prologue helpers were recognized as multiplication operators.

### Symbolic lifting of `main`
Instead of trying to fully emulate the ptrace/signal VM dynamically, the successful approach was to statically lift `main` into symbolic arithmetic.

Each live checker variable is represented as a linear expression in the 36 unknown input bytes:

```text
c0*x0 + c1*x1 + ... + c35*x35 + k
```

Registers and stack slots only need to be tracked enough to route pointers and expressions to helper calls. Conceptually:

```python
PTR = ('ptr', id)
EXPR = ('expr', linear_tuple)
```

The symbolic interpreter only needs to understand a small subset of behavior:

- register/stack motion (`mov`, `lea`, `movzx`, `movsxd`),
- variable allocation via `0x4036d0`,
- byte initialization via `0x4036f0`,
- addition via `0x403730`,
- subtraction via `0x403750`,
- direct helpers as `net` plus prefix constraint,
- prologue helpers as multiplication by a recovered constant.

Once that lifting is applied from the first counter allocation up to the success block, the challenge collapses to:

```text
36 unknown bytes
166 inequality constraints
36 equality constraints
```

The equalities come from the terminal checker block, and crucially they form a full-rank 36×36 system.

### Solving the linear system
The final equalities are assembled as a dense matrix:

```python
M = np.array([expr[:-1] for expr in equalities], dtype=np.float64)
b = np.array([-expr[-1] for expr in equalities], dtype=np.float64)
```

The notes recorded:

```text
rank = 36
condition number ≈ 104820.34580662058
```

That condition number is comfortably small enough that a direct floating-point solve works. The resulting solution vector rounds cleanly to integers with zero residual.

The recovered bytes are:

```text
[98, 99, 116, 102, 123, 98, 35, 102, 113, 81, 90, 42,
 79, 69, 81, 65, 90, 75, 84, 68, 65, 89, 121, 108,
 54, 86, 80, 82, 111, 109, 86, 80, 82, 111, 109, 125]
```

which decode to:

```text
bctf{b#fqQZ*OEQAZKTDAYyl6VPRomVPRom}
```

Notably, the corrected model did not need to assume a known `bctf{` prefix. The terminal system was already full-rank.

### Constraint validation
A good reverse solve must validate both the terminal equations and the underflow side-conditions. Plugging the recovered candidate back into the full symbolic model yields:

```text
min(ineq_res) = 0
max(abs(eq_res)) = 0
violated inequalities = none
number of tight inequalities = 37
```

So the candidate:

- satisfies all 36 equalities exactly,
- satisfies all 166 underflow inequalities,
- and never causes the VM to underflow.

This rules out the common failure mode where a candidate solves the terminal equations but would crash semantically during an earlier prefix pop.

### Why the challenge becomes linear algebra
At first glance this binary looks like classic anti-analysis pain:

- ptrace supervision,
- SIGBUS traps,
- strange disassembly,
- many helper functions at random addresses,
- and almost no obvious high-level structure.

But semantically it is much simpler:

- integers are linked-list lengths,
- faulting opcodes are `+1` or `-1`,
- helper functions are composed loops over those counters,
- loops over a source list become linear arithmetic in the source length,
- underflow restrictions become linear inequalities,
- final checks become linear equalities.

So the entire checker is equivalent to:

```text
A x = b
C x >= d
```

for a 36-byte input vector `x`.

The challenge becomes manageable precisely when you stop asking “what does this x86 instruction do?” and instead ask “what VM operation does the parent assign to this faulting opcode?”.

## Proof-of-Concept
- Step 1: Recover the VM model.

The essential semantic observations are:

```text
[r15+...] accesses are deliberate SIGBUS opcodes
0x403770 = push = +1
0x4037a0 = pop  = -1
0x403730 = dst += src
0x403750 = dst -= src
```

That reduces the child state to arithmetic over linked-list lengths.

- Step 2: Lift `main` symbolically into linear expressions over the 36 flag bytes.

The recovered final model contains:

```text
36 equalities
166 inequalities
```

and the equality matrix is full-rank.

A minimal solve is then:

```python
M = np.array([expr[:-1] for expr in equalities], dtype=np.float64)
b = np.array([-expr[-1] for expr in equalities], dtype=np.float64)
sol = np.linalg.solve(M, b)
flag = bytes(np.rint(sol).astype(int))
print(flag.decode())
```

- Step 3: Decode and verify the recovered flag.

```text
bctf{b#fqQZ*OEQAZKTDAYyl6VPRomVPRom}
```

## P/S
The hard part of this challenge is psychological rather than mathematical. The binary is built to look like anti-debugging chaos, but once the ptracing parent is treated as the real interpreter, the checker becomes a symbolic arithmetic problem with a clean linear-algebra core.