from pathlib import Path

a = Path("yuri.txt").read_text(encoding="utf-8")
b = Path("yuri_1.txt").read_text(encoding="utf-8")

flag = "".join(
    chr(ord(cb) - ord(ca))
    for ca, cb in zip(a, b)
    if ca != cb
)

print(flag)