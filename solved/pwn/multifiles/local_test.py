#!/usr/bin/env python3
import base64
import os
import select
import subprocess
import sys
import time


ROOT = "/workspace/multifiles"
KERNEL = os.path.join(ROOT, "build_out", "bzImage")
INITRD = os.path.join(ROOT, "build_out", "initrd.cpio.gz")
EXPLOIT = os.path.join(ROOT, "exploit_multifiles_musl")


def read_until(proc, needles, timeout=30.0, echo=True):
    end = time.time() + timeout
    buf = bytearray()
    while time.time() < end:
        remain = end - time.time()
        r, _, _ = select.select([proc.stdout], [], [], remain)
        if not r:
            continue
        chunk = os.read(proc.stdout.fileno(), 4096)
        if not chunk:
            break
        if echo:
            sys.stdout.buffer.write(chunk)
            sys.stdout.buffer.flush()
        buf += chunk
        for needle in needles:
            if needle in buf:
                return bytes(buf)
    raise TimeoutError(f"timed out waiting for {needles!r}")


def send_line(proc, line):
    proc.stdin.write(line + b"\n")
    proc.stdin.flush()


def send_blob(proc, guest_path, data):
    b64 = base64.encodebytes(data)
    send_line(proc, f"cat > {guest_path}.b64 <<'EOF'".encode())
    for line in b64.splitlines():
        proc.stdin.write(line + b"\n")
    proc.stdin.write(b"EOF\n")
    proc.stdin.flush()


def main():
    root_shell = os.environ.get("MF_ROOT_SHELL", "").strip() not in ("", "0")
    append = "console=ttyS0 loglevel=3 oops=panic panic=-1 pti=on kaslr"
    if root_shell:
        append += " rdinit=/bin/sh"

    proc = subprocess.Popen(
        [
            "qemu-system-x86_64",
            "-nodefaults",
            "-m",
            "256M",
            "-nographic",
            "-kernel",
            KERNEL,
            "-initrd",
            INITRD,
            "-append",
            append,
            "-cpu",
            "qemu64,+smep,+smap",
            "-smp",
            "1",
            "-no-reboot",
            "-serial",
            "stdio",
            "-monitor",
            "none",
        ],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        bufsize=0,
    )

    try:
        prompt = b"/ # " if root_shell else b"~ $ "
        read_until(proc, [prompt], timeout=20.0)
        if root_shell:
            send_line(proc, b"mount -t devtmpfs devtmpfs /dev")
            read_until(proc, [prompt], timeout=5.0)
            send_line(proc, b"mount -t proc proc /proc")
            read_until(proc, [prompt], timeout=5.0)
            send_line(proc, b"mount -t sysfs sysfs /sys")
            read_until(proc, [prompt], timeout=5.0)
            send_line(proc, b"/bin/insmod /multifiles.ko")
            read_until(proc, [prompt], timeout=5.0)
            send_line(proc, b"chmod 666 /dev/multifiles")
            read_until(proc, [prompt], timeout=5.0)
        send_line(proc, b"id")
        read_until(proc, [prompt], timeout=5.0)
        pre_cmds = os.environ.get("MF_ROOT_PRE_CMDS", "")
        if root_shell and pre_cmds.strip():
            for cmd in pre_cmds.split(";"):
                cmd = cmd.strip()
                if not cmd:
                    continue
                send_line(proc, cmd.encode())
                read_until(proc, [prompt], timeout=10.0)

        with open(EXPLOIT, "rb") as f:
            data = f.read()
        send_blob(proc, "/tmp/exploit", data)
        read_until(proc, [prompt], timeout=60.0, echo=False)

        send_line(proc, b"base64 -d /tmp/exploit.b64 >/tmp/exploit")
        read_until(proc, [prompt], timeout=20.0, echo=False)
        send_line(proc, b"chmod +x /tmp/exploit")
        read_until(proc, [prompt], timeout=10.0, echo=False)
        run_env = os.environ.get("MF_RUN_ENV", "").strip()
        if run_env:
            send_line(proc, (run_env + " /tmp/exploit").encode())
        else:
            send_line(proc, b"/tmp/exploit")

        timeout = float(os.environ.get("MF_TIMEOUT", "60"))
        needles = [b"FLAG=", b"Kernel panic", b"Rebooting in"]
        if os.environ.get("MF_WAIT_SIGNAL", "").strip() not in ("", "0"):
            needles.append(b"[!] signal")
        if os.environ.get("MF_WAIT_FOREIGN", "").strip() not in ("", "0"):
            needles.append(b"foreign errno")
        read_until(proc, needles, timeout=timeout)
        time.sleep(1.0)
    finally:
        proc.kill()
        proc.wait()


if __name__ == "__main__":
    main()
