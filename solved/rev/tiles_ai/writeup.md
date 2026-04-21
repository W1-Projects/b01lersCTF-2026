# tiles + ai

**Category:** Rev

## Description
> Attachment: `revc`
>
> `ncat --ssl tiles--ai.opus4-7.b01le.rs 8443`

## Overview
The target is a stripped static ELF that hard-gates execution on Intel AMX support. The core logic is not an AMX reversing challenge, though: the binary embeds three `6x6` Rush Hour boards and validates a sequence of moves encoded as repeated hex pairs. The only real checks are: each move must respect the piece orientation, the resulting board must stay non-overlapping, and piece `0` must reach the exit cell. AMX is only used as an obfuscated matrix engine for state transitions.

## Technical Details & Root Cause
The top-level control flow is visible directly in `.text`: CPU gate, per-round prompt, input validation, and final `flag.txt` read.

```asm
4011a2: mov    edi,0x9e
4011a7: mov    esi,0x1023
4011ac: mov    edx,0x12
4011b3: call   0x403942
4011b8: test   rax,rax
4011bb: jne    0x4011d8
4011bd: mov    edi,0x9e
4011c2: mov    esi,0x1023
4011c7: mov    edx,0x11
4011ce: call   0x403942
4011d3: test   rax,rax
4011d6: je     0x401211
4011d8: mov    edi,0x4101b0                  ; "Cannot run on this CPU"
...
4012dc: mov    edi,0x410194                  ; "%d> "
...
401ca4: mov    edi,0x4101a5                  ; "flag.txt"
401ca9: mov    esi,0x4101ae                  ; "r"
...
401cee: mov    edi,0x410184                  ; "flag{fake_flag}"
401d04: mov    edi,0x41019b                  ; "incorrect"
```

The relevant strings are present in `dist/export-for-ai/strings.txt`:

```text
0x410184 | ASCII | flag{fake_flag}
0x410194 | ASCII | %d>
0x41019b | ASCII | incorrect
0x4101a5 | ASCII | flag.txt
0x4101b0 | ASCII | Cannot run on this CPU
```

Each round starts by copying exactly `0x300` bytes from an embedded table at `0x40f800 + round * 0x300` into the live state buffer at `0x412270`.

```asm
401276: mov    rax,QWORD PTR [rsp+0x8]       ; round index
40127b: lea    rax,[rax+rax*2]
40127f: shl    rax,0x8                       ; round * 0x300
401283: lea    rsi,[rax+0x40f800]            ; source
...
4012ca: mov    edi,0x412270                  ; destination
4012cf: mov    edx,0x300
4012d7: call   0x406300                      ; memcpy
```

`0x300 = 768 = 36 * 16 * 1`, which matches a `6x6` board with 36 cells and a one-hot row of 16 possible piece IDs per cell. `solve.py` reconstructs exactly that layout:

```python
BASE_ADDR = 0x400000
ROUND_STATE_ADDR = 0x40F800
ROUND_STATE_SIZE = 0x300

for round_id in range(3):
    off = ROUND_STATE_ADDR - BASE_ADDR + round_id * ROUND_STATE_SIZE
    chunk = data[off : off + ROUND_STATE_SIZE]
    for cell in range(36):
        row = chunk[cell * 16 : (cell + 1) * 16]
```

A direct extraction of the embedded states confirms normal Rush Hour pieces, not arbitrary AMX data:

```text
round 0 {'0x0': (12, 13), '0x1': (25, 26, 27), '0xe': (18, 24, 30), '0xf': (3, 9, 15)}
round 1 {'0x0': (14, 15), '0x1': (22, 23), '0x2': (3, 4, 5), '0x3': (6, 7), '0x4': (9, 10), '0x5': (30, 31, 32), '0x8': (21, 27, 33), '0xa': (28, 34), '0xd': (13, 19, 25), '0xe': (11, 17), '0xf': (2, 8)}
round 2 {'0x0': (12, 13), '0x1': (19, 20, 21), '0x2': (3, 4, 5), '0x3': (33, 34), '0x9': (9, 15), '0xa': (17, 23), '0xb': (10, 16, 22), '0xc': (29, 35), '0xd': (26, 32), '0xe': (1, 7), '0xf': (18, 24, 30)}
```

The input format is also explicit in assembly. The program parses two ASCII hex nibbles at a time: the first nibble becomes the piece ID, the second must be `<= 3` and becomes the direction.

```asm
401366: movsx  eax,BYTE PTR [rdi+rsi*1]      ; first char
...
4013a5: cmovae eax,ecx                       ; invalid -> -1
4013a8: movsx  ecx,BYTE PTR [rdi+rsi*1+0x1]  ; second char
...
4013d5: cmovae ecx,edx                       ; invalid -> -1
4013d8: test   eax,eax
4013da: js     0x401cff
4013e0: cmp    ecx,0x3
4013e3: ja     0x401cff
```

That is why the solver emits move strings like `01e2...`: one hex digit for the piece and one hex digit for the direction. The solver reproduces the same convention:

```python
DIRS = {
    0: (0, -1),  # left
    1: (0, 1),   # right
    2: (-1, 0),  # up
    3: (1, 0),   # down
}
...
queue.append((next_state, path + f"{piece_id:x}{direction}"))
```

The AMX table selection logic is the key evidence that the binary is applying precomputed move matrices, not doing arbitrary arithmetic. After parsing one move, it computes three table bases:

```asm
4013ee: mov    edx,eax                        ; piece_id
4013f0: shr    eax,0x3                        ; 0 for 0..7, 1 for 8..15
4013f5: lea    rcx,[rcx+rcx*8]
4013f9: shl    rcx,0x8                        ; direction * 0x900
4013fd: lea    rax,[rax+rax*8]
401401: shl    rax,0xa                        ; orientation * 0x2400
401405: lea    r15,[rax+rcx*1+0x40b000]       ; move-table family
40140d: shl    rdx,0x8                        ; piece_id * 0x100
401411: lea    rax,[rdx+0x409000]             ; selector matrix
40141d: lea    rax,[rdx+0x40a000]             ; keep-other-pieces matrix
```

This address arithmetic already leaks the structure of the move tables:

- `0x409000 + piece_id * 0x100`: one selector block per piece.
- `0x40a000 + piece_id * 0x100`: one "keep everything except this piece" block per piece.
- `0x40b000 + orientation * 0x2400 + direction * 0x900`: `2 * 4` move families, each containing `9` sub-blocks of `0x100` bytes.

The raw `.rodata` supports that interpretation. At `0x409000`, the first few `0x100`-byte blocks are one-hot selectors:

```text
409000 01000000 00000000 00000000 00000000
...
409100 00000000 00010000 00000000 00000000
...
409200 00000000 00000000 00000100 00000000
```

And a quick dump over the binary confirms the pattern:

```text
pick
0 [0] count=1
1 [1] count=1
2 [2] count=1
3 [3] count=1
```

At `0x40a000`, the corresponding blocks keep 15 diagonal entries and drop the selected one:

```text
40a000 00000000 00010000 00000100 00000001
...
40a100 01000000 00000000 00000100 00000001
...
40a200 01000000 00010000 00000000 00000001
```

Again, a direct dump over the binary shows `count=15` non-zero diagonal slots for each block:

```text
keep
0 count=15
1 count=15
2 count=15
3 count=15
```

So the root cause is not memory corruption. The binary encodes legal board transitions as matrix transforms, and the "obfuscation boundary" is simply recognizing that these transforms implement Rush Hour movement.

The post-move validation loop proves the game semantics. It walks the state row-by-row, sums the 16 bytes of each cell, and rejects any cell with total occupancy `>= 2`.

```asm
401b16: movzx  ecx,BYTE PTR [rax+0x4124b0]
401b25: movzx  edx,BYTE PTR [rax+0x4124b1]
...
401c4d: add    ebx,r8d
401c50: cmp    ebx,0x2
401c53: jae    0x401cfa                         ; overlap -> incorrect
401c59: add    rax,0x10                         ; next cell
401c5d: jne    0x401b16
```

Finally, the win condition is a single byte check:

```asm
401c84: cmp    BYTE PTR [rip+0x106f5],0x1       ; 0x412380
401c8b: jne    0x401d04
```

Since the live state starts at `0x412270` and each cell is `0x10` bytes, this address is:

```text
0x412380 - 0x412270 = 0x110 = 17 * 0x10
```

So `0x412380` is the first byte of cell `17`, i.e. the occupancy bit for piece `0` at cell `17`. That matches the solver constant:

```python
TARGET_CELL = 17
if TARGET_CELL in state.get(0, ()):
    return path
```

## Proof-of-Concept / Exploit Strategy
- Step 1: Extract the three embedded boards from `dist/revc`.
  Purpose: avoid AMX execution entirely and lift the real puzzle state from `.rodata`.
  Payload structure: none yet; this step reconstructs a `dict[piece_id] -> tuple[cells]` from the `36 x 16` one-hot state.
```python
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
```

- Step 2: Rebuild the legal move generator from the parser and AMX table selection logic.
  Purpose: reproduce exactly what the binary accepts as one move.
  Payload structure: one move is `<piece_id><direction>`, both encoded as hex nibbles; `piece_id < 8` is horizontal and only allows `0/1`, while `piece_id >= 8` is vertical and only allows `2/3`.
```python
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
```

- Step 3: Run BFS until piece `0` reaches cell `17`.
  Purpose: solve each board optimally without emulating AMX.
  Payload structure: the final answer for one round is the concatenation of all 2-hex-digit moves in BFS order.
```python
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
```

- Step 4: Submit the three round solutions to the remote service.
  Purpose: the local binary only contains the fake flag fallback; the real flag is returned by the remote checker after all three prompts are solved.
  Payload structure: three newline-terminated move strings, one per prompt `0>`, `1>`, `2>`.
```python
def submit(host: str, port: int, solutions: list[str]) -> str:
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    with socket.create_connection((host, port), timeout=10) as raw:
        with ctx.wrap_socket(raw, server_hostname=host) as sock:
            recv_until(sock, (b"> ",))
            out = []
            for line in solutions:
                sock.sendall(line.encode() + b"\n")
                out.append(recv_until(sock, (b"> ", b"flag{", b"bctf{", b"incorrect")))
            return b"".join(out).decode("utf-8", "replace")
```

The solver recovers the exact accepted move strings:

```text
round 0: 01e2e210f3f3f3010101
round 1: 01f320e201
round 2: 0120a2a2c231f2f2f2109393019311e320b211e3e300e31010923092921111c311d230e23030d310f3209201e2e210b3b3b30101
```

Submitting them to `tiles--ai.opus4-7.b01le.rs:8443` returns:

```text
0> 1> 2> bctf{in_the_matrix_straight_up_multiplying_it_ec3428a06}
```

## P/S
The intended difficulty was the AMX presentation layer. The actual shortest path was to treat `revc` as a data container: lift the state from `.rodata`, identify the puzzle from the occupancy model and exit check, then solve it offline as a standard Rush Hour BFS.
