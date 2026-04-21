#!/usr/bin/env python3

import math
import re
import socket
import ssl
from functools import lru_cache


HOST = "sporadiclogarithms.opus4-7.b01le.rs"
PORT = 8443
PROMPT = b"bb> "
BATCH = 64


class Remote:
    def __init__(self, host: str, port: int) -> None:
        raw = socket.create_connection((host, port))
        ctx = ssl.create_default_context()
        self.sock = ctx.wrap_socket(raw, server_hostname=host)
        self.buf = b""

    def recv_until(self, marker: bytes) -> bytes:
        while marker not in self.buf:
            chunk = self.sock.recv(65536)
            if not chunk:
                raise EOFError(self.buf.decode("utf-8", "replace"))
            self.buf += chunk
        idx = self.buf.index(marker) + len(marker)
        out = self.buf[:idx]
        self.buf = self.buf[idx:]
        return out

    def recv_prompts(self, count: int) -> bytes:
        seen = self.buf.count(PROMPT)
        while seen < count:
            chunk = self.sock.recv(65536)
            if not chunk:
                raise EOFError(self.buf.decode("utf-8", "replace"))
            self.buf += chunk
            seen = self.buf.count(PROMPT)

        end = 0
        for _ in range(count):
            end = self.buf.index(PROMPT, end) + len(PROMPT)

        out = self.buf[:end]
        self.buf = self.buf[end:]
        return out

    def sendline(self, line: str) -> None:
        self.sock.sendall(line.encode() + b"\n")

    def sendlines(self, lines: list[str]) -> None:
        data = "".join(f"{line}\n" for line in lines)
        self.sock.sendall(data.encode())

    def close(self) -> None:
        self.sock.close()


class Solver:
    def __init__(self, tube: Remote) -> None:
        self.tube = tube
        self.round = 0
        self.one = 1
        self.bound = 0
        self.max_handle = 0

    @staticmethod
    def parse_int(output: str) -> int | None:
        nums = re.findall(r"(?m)^(-?\d+)\r?$", output)
        return int(nums[-1]) if nums else None

    def note_handle(self, handle: int | None) -> None:
        if handle is not None and handle > self.max_handle:
            self.max_handle = handle

    def read_prompt_block(self) -> str:
        return self.tube.recv_until(PROMPT).decode("utf-8", "replace")

    def command(self, line: str) -> str:
        self.tube.sendline(line)
        return self.read_prompt_block()

    def command_many(self, lines: list[str]) -> list[str]:
        self.tube.sendlines(lines)
        raw = self.tube.recv_prompts(len(lines)).decode("utf-8", "replace")
        parts = raw.split(PROMPT.decode())
        return parts[:-1]

    def command_int(self, line: str) -> int:
        out = self.command(line)
        value = self.parse_int(out)
        if value is None:
            raise ValueError(f"missing integer result for {line!r}: {out!r}")
        self.note_handle(value)
        return value

    @lru_cache(maxsize=None)
    def inv(self, handle: int) -> int:
        return self.command_int(f"inv {handle}")

    def mul_chain(self, cur: int, rhs: int, count: int) -> list[int]:
        out = []
        remaining = count

        while remaining > 0:
            batch = min(BATCH, remaining)
            next_new = self.max_handle + 1
            lines = []
            predicted = []
            pred_cur = cur

            for i in range(batch):
                lines.append(f"mul {pred_cur} {rhs}")
                predicted_handle = next_new + i
                predicted.append(predicted_handle)
                pred_cur = predicted_handle

            vals = []
            for piece in self.command_many(lines):
                val = self.parse_int(piece)
                self.note_handle(val)
                vals.append(val)

            usable = 0
            for i, val in enumerate(vals):
                if val is None:
                    if usable == i:
                        raise ValueError(f"unexpected non-int result in chain: {lines[i]!r}")
                    break

                out.append(val)
                cur = val
                usable += 1

                if val != predicted[i]:
                    break

            remaining -= usable

        return out

    def parse_round(self, banner: str) -> tuple[int, int, int, int, int]:
        m = int(re.search(r"c order=(\d+)", banner).group(1))
        self.bound = int(re.search(r"Find any x in \[0, (\d+)\]", banner).group(1))
        one, g, c, h = map(
            int,
            re.search(r"one=(\d+) g=(\d+) c=(\d+) h=(\d+)", banner).groups(),
        )
        self.one = one
        self.max_handle = max(one, g, c, h)
        return m, one, g, c, h

    def twisted_residue(self, g: int, h: int, terms: list[int]) -> list[int]:
        inv_h = self.inv(h)
        phi_h = self.command_int(f"phi {h}")
        tmp = self.command_int(f"mul {inv_h} {g}")
        witness = self.command_int(f"mul {tmp} {phi_h}")
        return [r for r, term in enumerate(terms) if term == witness]

    def build_prefix(self, one: int, terms: list[int], upto: int) -> list[int]:
        prefix = [one]
        cur = one
        for i in range(upto):
            cur = self.command_int(f"mul {cur} {terms[i]}")
            prefix.append(cur)
        return prefix

    def solve_dlog(self, base: int, target: int, limit: int) -> int | None:
        step = math.isqrt(limit) + 1
        powers = [self.one] + self.mul_chain(self.one, base, step)
        table = {}
        for j, handle in enumerate(powers[:-1]):
            table.setdefault(handle, j)

        factor = self.inv(powers[-1])
        gamma = target
        j = table.get(gamma)
        if j is not None:
            return j

        max_i = limit // step + 1
        giant = self.mul_chain(gamma, factor, max_i)
        for i, handle in enumerate(giant, start=1):
            j = table.get(handle)
            if j is None:
                continue
            cand = i * step + j
            if cand <= limit:
                return cand
        return None

    def solve_round(self, banner: str) -> None:
        self.inv.cache_clear()
        self.round += 1
        m, one, g, _c, h = self.parse_round(banner)
        print(f"[+] round {self.round}: m={m}, bound={self.bound}", flush=True)

        terms = [g]
        for _ in range(1, m):
            terms.append(self.command_int(f"phi {terms[-1]}"))

        residues = self.twisted_residue(g, h, terms)
        if not residues:
            raise RuntimeError("could not determine x mod ord(c)")

        prefix = self.build_prefix(one, terms, m)
        block = prefix[m]

        for r in residues:
            target = self.command_int(f"mul {h} {self.inv(prefix[r])}")
            q = self.solve_dlog(block, target, (self.bound - r) // m)
            if q is None:
                continue

            answer = q * m + r
            print(f"[+] submit x={answer}", flush=True)
            self.tube.sendline(f"submit {answer}")
            return

        raise RuntimeError("no valid exponent found")


def main() -> None:
    tube = Remote(HOST, PORT)
    solver = Solver(tube)

    try:
        while True:
            banner = solver.read_prompt_block()
            solver.solve_round(banner)
    except EOFError as exc:
        tail = str(exc).strip()
        if tail:
            print(tail)
    finally:
        tube.close()


if __name__ == "__main__":
    main()
