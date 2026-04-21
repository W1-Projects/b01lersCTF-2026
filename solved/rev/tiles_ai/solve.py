#!/usr/bin/env python3
import argparse
import socket
import ssl
from collections import deque
from pathlib import Path


BASE_ADDR = 0x400000
ROUND_STATE_ADDR = 0x40F800
ROUND_STATE_SIZE = 0x300
TARGET_CELL = 17

DIRS = {
    0: (0, -1),  # left
    1: (0, 1),   # right
    2: (-1, 0),  # up
    3: (1, 0),   # down
}


def parse_rounds(binary_path: Path) -> list[dict[int, tuple[int, ...]]]:
    data = binary_path.read_bytes()
    rounds = []
    for round_id in range(3):
        off = ROUND_STATE_ADDR - BASE_ADDR + round_id * ROUND_STATE_SIZE
        chunk = data[off : off + ROUND_STATE_SIZE]
        pieces: dict[int, list[int]] = {}
        for cell in range(36):
            row = chunk[cell * 16 : (cell + 1) * 16]
            for piece_id, value in enumerate(row):
                if value:
                    pieces.setdefault(piece_id, []).append(cell)
        rounds.append({piece_id: tuple(sorted(cells)) for piece_id, cells in pieces.items()})
    return rounds


def to_grid(pieces: dict[int, tuple[int, ...]]) -> list[list[int]]:
    grid = [[-1] * 6 for _ in range(6)]
    for piece_id, cells in pieces.items():
        for cell in cells:
            y, x = divmod(cell, 6)
            if grid[y][x] != -1:
                raise ValueError("overlapping pieces in state")
            grid[y][x] = piece_id
    return grid


def canonical_state(pieces: dict[int, tuple[int, ...]]) -> tuple[tuple[int, tuple[int, ...]], ...]:
    return tuple((piece_id, pieces[piece_id]) for piece_id in sorted(pieces))


def move_piece(pieces: dict[int, tuple[int, ...]], piece_id: int, direction: int):
    horizontal = piece_id < 8
    if horizontal and direction not in (0, 1):
        return None
    if not horizontal and direction not in (2, 3):
        return None

    dy, dx = DIRS[direction]
    grid = to_grid(pieces)
    moved_cells = []
    for cell in pieces[piece_id]:
        y, x = divmod(cell, 6)
        ny, nx = y + dy, x + dx
        if not (0 <= ny < 6 and 0 <= nx < 6):
            return None
        occupant = grid[ny][nx]
        if occupant != -1 and occupant != piece_id:
            return None
        moved_cells.append(ny * 6 + nx)

    moved_cells = tuple(sorted(moved_cells))
    ys = sorted({cell // 6 for cell in moved_cells})
    xs = sorted({cell % 6 for cell in moved_cells})
    if horizontal:
        if len(ys) != 1 or xs != list(range(xs[0], xs[-1] + 1)):
            return None
    else:
        if len(xs) != 1 or ys != list(range(ys[0], ys[-1] + 1)):
            return None

    next_state = dict(pieces)
    next_state[piece_id] = moved_cells
    all_cells = [cell for cells in next_state.values() for cell in cells]
    if len(all_cells) != len(set(all_cells)):
        return None
    return next_state


def solve_round(initial_state: dict[int, tuple[int, ...]]) -> str:
    queue = deque([(initial_state, "")])
    seen = {canonical_state(initial_state)}

    while queue:
        state, path = queue.popleft()
        if TARGET_CELL in state.get(0, ()):
            return path
        for piece_id in sorted(state):
            for direction in range(4):
                next_state = move_piece(state, piece_id, direction)
                if next_state is None:
                    continue
                key = canonical_state(next_state)
                if key in seen:
                    continue
                seen.add(key)
                queue.append((next_state, path + f"{piece_id:x}{direction}"))
    raise RuntimeError("no solution found")


def recv_until(sock: ssl.SSLSocket, markers: tuple[bytes, ...]) -> bytes:
    data = b""
    while True:
        chunk = sock.recv(4096)
        if not chunk:
            return data
        data += chunk
        if any(marker in data for marker in markers):
            return data


def submit(host: str, port: int, solutions: list[str]) -> str:
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    with socket.create_connection((host, port), timeout=10) as raw:
        with ctx.wrap_socket(raw, server_hostname=host) as sock:
            sock.settimeout(10)
            recv_until(sock, (b"> ",))
            out = []
            for line in solutions:
                sock.sendall(line.encode() + b"\n")
                out.append(recv_until(sock, (b"> ", b"flag{", b"bctf{", b"incorrect")))
            return b"".join(out).decode("utf-8", "replace")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", default="revc")
    parser.add_argument("--submit", action="store_true")
    parser.add_argument("--host", default="tiles--ai.opus4-7.b01le.rs")
    parser.add_argument("--port", type=int, default=8443)
    args = parser.parse_args()

    rounds = parse_rounds(Path(args.binary))
    solutions = [solve_round(state) for state in rounds]
    for idx, line in enumerate(solutions):
        print(f"round {idx}: {line}")

    if args.submit:
        print(submit(args.host, args.port, solutions), end="")


if __name__ == "__main__":
    main()
