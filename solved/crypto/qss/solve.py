#!/usr/bin/env python3
from __future__ import annotations

import json
import math
import socket
import ssl
import sys
from typing import List


HOST = "qss.opus4-7.b01le.rs"
PORT = 8443

# ancilla = |0>
ANCILLA = [1.0, 0.0]

# U = SWAP(a, c) · H(a) · CNOT(a -> b), in the user's abc basis order.
SQRT2_INV = 1.0 / math.sqrt(2.0)
UNITARY = [
    [SQRT2_INV, 0.0, 0.0, 0.0, 0.0, 0.0, SQRT2_INV, 0.0],
    [SQRT2_INV, 0.0, 0.0, 0.0, 0.0, 0.0, -SQRT2_INV, 0.0],
    [0.0, 0.0, SQRT2_INV, 0.0, SQRT2_INV, 0.0, 0.0, 0.0],
    [0.0, 0.0, SQRT2_INV, 0.0, -SQRT2_INV, 0.0, 0.0, 0.0],
    [0.0, SQRT2_INV, 0.0, 0.0, 0.0, 0.0, 0.0, SQRT2_INV],
    [0.0, SQRT2_INV, 0.0, 0.0, 0.0, 0.0, 0.0, -SQRT2_INV],
    [0.0, 0.0, 0.0, SQRT2_INV, 0.0, SQRT2_INV, 0.0, 0.0],
    [0.0, 0.0, 0.0, SQRT2_INV, 0.0, -SQRT2_INV, 0.0, 0.0],
]


class Remote:
    def __init__(self, host: str, port: int):
        self.host = host
        self.port = port
        self.sock: ssl.SSLSocket | None = None
        self.file = None

    def __enter__(self) -> "Remote":
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE

        raw = socket.create_connection((self.host, self.port), timeout=10)
        self.sock = ctx.wrap_socket(raw, server_hostname=self.host)
        self.sock.settimeout(30)
        self.file = self.sock.makefile("rwb", buffering=0)
        return self

    def __exit__(self, exc_type, exc, tb) -> None:
        try:
            if self.file is not None:
                self.file.close()
        finally:
            if self.sock is not None:
                self.sock.close()

    def recvline(self) -> str:
        if self.file is None:
            raise RuntimeError("connection is closed")
        line = self.file.readline()
        if not line:
            raise EOFError("connection closed")
        return line.decode("utf-8", errors="replace").rstrip("\n")

    def sendline(self, text: str) -> None:
        if self.file is None:
            raise RuntimeError("connection is closed")
        self.file.write(text.encode() + b"\n")


def choose_basis(counter: int) -> str:
    return "z" if counter % 2 == 0 else "x"


def run_once(attempt: int) -> str | None:
    with Remote(HOST, PORT) as io:
        line = io.recvline()
        if "ancilla_statevector_json>" not in line:
            raise RuntimeError(f"unexpected prompt: {line!r}")
        io.sendline(json.dumps(ANCILLA))

        line = io.recvline()
        if "unitary_8x8_json>" not in line:
            raise RuntimeError(f"unexpected prompt: {line!r}")
        io.sendline(json.dumps(UNITARY))

        test_counter = 0
        info_counter = 0

        while True:
            line = io.recvline()
            if line.startswith("Input error:"):
                raise RuntimeError(line)
            if line.startswith("Missing dependency:"):
                raise RuntimeError(line)
            if line == "":
                continue
            if line == "=== score ===":
                score_lines = [line]
                for _ in range(17):
                    score_lines.append(io.recvline())
                transcript = "\n".join(score_lines)
                print(f"[attempt {attempt}] final:")
                print(transcript)
                final_line = score_lines[-1]
                if final_line.startswith("No flag yet:") or final_line.startswith("ABORT:"):
                    return None
                return final_line

            if not line.startswith("--- round "):
                continue

            round_type = io.recvline().split(": ", 1)[1]
            if round_type == "test":
                basis = choose_basis(test_counter)
                test_counter += 1
            else:
                basis = choose_basis(info_counter)
                info_counter += 1

            io.recvline()  # pre_public_announce_json>
            io.sendline(json.dumps({"basis": basis, "outcome": 0}))

            public_set = io.recvline().split(": ", 1)[1]
            alice_basis = io.recvline().split(": ", 1)[1]

            if round_type == "test":
                io.recvline()  # test_check_valid: ...
                continue

            info_keep = io.recvline().split(": ", 1)[1]
            if info_keep != "yes":
                continue

            if public_set.startswith("S0"):
                io.recvline()  # measurement_plan_json>
                io.sendline(json.dumps([["b", "z"]]))
                outcomes = io.recvline()
                secret = int(outcomes.rsplit("=", 1)[1])
            else:
                io.recvline()  # measurement_plan_json>
                io.sendline(json.dumps([["b", "x"], ["c", "x"]]))
                outcomes = io.recvline()
                parts = outcomes.split()
                b_bit = int(parts[1].rsplit("=", 1)[1])
                c_bit = int(parts[2].rsplit("=", 1)[1])
                secret = int(b_bit == c_bit)

            io.recvline()  # secret_guess_bit>
            io.sendline(str(secret))


def main() -> None:
    max_attempts = 200
    for attempt in range(1, max_attempts + 1):
        try:
            flag = run_once(attempt)
            if flag is not None:
                print(flag)
                return
        except Exception as exc:
            print(f"[attempt {attempt}] error: {exc}", file=sys.stderr)
    raise SystemExit(f"no flag after {max_attempts} attempts")


if __name__ == "__main__":
    main()
