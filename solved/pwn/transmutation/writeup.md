# Transmutation

**Category:** Pwn

## Description

> To turn one program into another... is it even possible?
>
> `ncat --ssl transmutation.opus4-7.b01le.rs 8443`

## Overview

A minimal C program reads two bytes of input and writes one byte into its own `.text` section. The code page is made RWX by `mprotect` before the write happens. The challenge is a **self-modifying code** puzzle: bootstrap a single 1-byte write primitive into full shellcode execution by surgically patching the program's own instructions.

## Technical Details

### Source Analysis

```c
void chall(void) {
    char c = getchar();
    unsigned char i = getchar();
    if (i < LEN) {
        CHALL[i] = c;
    }
}

int main(void) {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    mprotect((char *)((long)CHALL & ~0xfff), 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
    chall();
    return 0;
}
```

Key observations:

- `main` calls `mprotect` to make the page containing `chall` and `main` **RWX**.
- `chall()` reads exactly 2 bytes: a value `c` and an offset `i`.
- If `i < LEN` (where `LEN = main - chall = 0x49`), it writes `c` to `chall[i]` -- i.e., directly into the `.text` segment.
- `chall()` is called **once**, then `main` returns. So the initial primitive is a single 1-byte write into the function's own machine code.

### Mitigations

| Protection | Status |
|---|---|
| PIE | No |
| RELRO | Partial |
| Canary | No |
| NX | Yes, but irrelevant -- `.text` page is RWX |
| seccomp | None |

### Key Addresses

| Symbol | Address | Note |
|---|---|---|
| `chall` | `0x401146` | Start of writable function |
| `chall+0x48` (ret) | `0x40118e` | `0xc3` -- function epilogue return |
| `chall+0x31` (jge disp) | `0x401177` | `0x14` -- branch displacement byte |
| `main` | `0x40118f` | Immediately after `chall`'s `ret` |
| `call chall` return site | `0x4011f3` | Where shellcode is staged |

### Exploit Strategy

The exploit has three stages, each using the write primitive to patch a single byte:

**Stage 1 -- Create reentry (patch `ret` to fallthrough)**

The last three bytes of `chall` are:

```
40118c: 90    nop
40118d: c9    leave
40118e: c3    ret
40118f: 55    push rbp   ; first byte of main
```

Patch `chall+0x48` from `0xc3` to `0x48`. The CPU now sees `0x48 0x55` which is `push rbp` with a REX.W prefix -- execution **falls through into `main+1`** instead of returning. `main` re-runs (including `mprotect` and another `call chall`), giving us **repeated invocations** of `chall()` from a single input stream.

**Stage 2 -- Expand write primitive (patch branch displacement)**

The bounds check in `chall`:

```
401173: 48 39 c2    cmp  rdx, rax
401176: 7d 14       jge  0x40118c    ; skip store if i >= LEN
```

Patch `chall+0x31` from `0x14` to `0x00`. The branch becomes `jge +0` (jumps to the next instruction), so the **store always executes regardless of offset**. This upgrades the primitive from "write within `chall`" to "write anywhere reachable from `chall` base + unsigned byte offset" -- enough to reach `main`'s code.

**Stage 3 -- Stage ORW shellcode and restore return**

With the expanded write primitive and repeated invocations, write an open-read-write shellcode byte-by-byte starting at offset `0xAD` from `chall` (address `0x4011f3` -- the instruction right after `call chall` in `main`).

Once shellcode is fully staged, patch `chall+0x48` back from `0x48` to `0xc3`, restoring the normal `ret`. Now `chall` returns normally, and execution lands on our shellcode at `0x4011f3`.

The shellcode performs:

1. `open("/app/flag.txt", O_RDONLY)` (or `"flag.txt"` locally)
2. `read(fd, stack_buf, 64)`
3. `write(1, stack_buf, n)`
4. `exit(0)`

### Payload Structure

The entire payload is a flat stream of `(value, offset)` byte pairs:

```
[0x48, 0x48]                      # Stage 1: ret -> fallthrough
[0x00, 0x31]                      # Stage 2: expand write primitive
[sc[0], 0xAD] [sc[1], 0xAE] ...  # Stage 3: write shellcode bytes
[0xC3, 0x48]                      # Restore ret -> trigger shellcode
```

## Proof-of-Concept

Full exploit: [`exploit.py`](exploit.py)

```bash
# Local
python3 exploit.py

# Remote
python3 exploit.py --remote
```

Local result:

```
bctf{fake_flag}
```

Remote result:

```
bctf{CPU_0pt1m1z3r5_H4T3_th15_0n3_51mp13_tr1ck_5519225335}
```

## P/S

- The core insight is recognizing this is a self-modifying code challenge, not a traditional memory corruption problem. There is no need for leaks, ROP chains, or libc -- the `.text` page is already RWX.
- The unlock sequence is: **weak primitive -> patch own logic to create reentry -> patch own logic to remove bounds check -> strong primitive -> stage shellcode -> restore control flow**.
- ORW was chosen over spawning a shell for stability on the remote jailed environment.
- A wrong turn early on was trying to directly patch jump targets around the epilogue for looping. The real key was modifying the **write path logic** (the `jge` displacement) rather than only the control flow at function end.
