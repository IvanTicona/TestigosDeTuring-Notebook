#!/usr/bin/env python3
"""Upload the notebook PDF to an existing Google Drive file.

Updates the file CONTENT (media-only PATCH) to preserve the fileId, name
and shared links. Never prints secrets or tokens.

Environment:
  GDRIVE_FILE_ID                      Drive file id (required)
  GDRIVE_SA_JSON                      service account JSON (primary auth)
  GDRIVE_OAUTH_CLIENT_ID / _SECRET / _REFRESH_TOKEN
                                      OAuth fallback if GDRIVE_SA_JSON is not set

Usage:
  python scripts/drive_upload.py --file build/testigos-de-turing-teambook.pdf
  python scripts/drive_upload.py --check     # only verifies permissions (no upload)

Exit: 0 ok, or skipped for missing secrets (upload only); 1 failure.
google-auth and requests are imported lazily (see requirements-drive.txt).
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Any, Mapping

EXPECTED_NAME = "testigos-de-turing-teambook.pdf"
SCOPE = "https://www.googleapis.com/auth/drive"  # drive.file does not work: the file was not created by the app
TOKEN_URI = "https://oauth2.googleapis.com/token"
UPLOAD_URL = (
    "https://www.googleapis.com/upload/drive/v3/files/{id}"
    "?uploadType=media&supportsAllDrives=true&fields=id,name,modifiedTime,size"
)
CHECK_URL = (
    "https://www.googleapis.com/drive/v3/files/{id}"
    "?supportsAllDrives=true&fields=id,name,owners,capabilities/canEdit"
)
OAUTH_VARS = ("GDRIVE_OAUTH_CLIENT_ID", "GDRIVE_OAUTH_CLIENT_SECRET", "GDRIVE_OAUTH_REFRESH_TOKEN")
SKIP_MESSAGE = "Drive skipped: missing secrets"


class DriveError(Exception):
    """Drive failure with a safe message (no secrets)."""


# --------------------------------------------------------------------------
# Pure logic
# --------------------------------------------------------------------------
def has_oauth(env: Mapping[str, str]) -> bool:
    return all(env.get(k) for k in OAUTH_VARS)


def skip_reason(env: Mapping[str, str]) -> str | None:
    """Skip message if secrets are missing; None if it can proceed."""
    if not env.get("GDRIVE_FILE_ID"):
        return f"{SKIP_MESSAGE} (GDRIVE_FILE_ID)"
    if not env.get("GDRIVE_SA_JSON") and not has_oauth(env):
        return f"{SKIP_MESSAGE} (GDRIVE_SA_JSON or GDRIVE_OAUTH_*)"
    return None


def sa_client_email(env: Mapping[str, str]) -> str | None:
    """client_email of the SA (the only SA field that may be shown)."""
    raw = env.get("GDRIVE_SA_JSON")
    if not raw:
        return None
    try:
        email = json.loads(raw).get("client_email")
    except (ValueError, AttributeError):
        return None
    return email if isinstance(email, str) else None


def error_reason(body: Any) -> str:
    """Extract error.errors[0].reason from an API error body."""
    try:
        return str(body["error"]["errors"][0]["reason"])
    except (KeyError, IndexError, TypeError):
        pass
    try:
        return str(body["error"]["status"])
    except (KeyError, TypeError):
        return "unknown"


def map_error(status: int, reason: str, client_email: str | None = None) -> str:
    """Actionable message for a non-2xx HTTP status (pure function)."""
    who = client_email or "the account in use"
    head = f"Drive HTTP {status} ({reason}): "
    if status == 401:
        return head + "invalid or expired credentials (check GDRIVE_SA_JSON / GDRIVE_OAUTH_*)"
    if status == 403 and reason == "insufficientFilePermissions":
        return head + f"share the file with {who} as Editor"
    if status == 403 and reason == "storageQuotaExceeded":
        return head + (
            "the service account has no storage quota; "
            "use the OAuth fallback (GDRIVE_OAUTH_CLIENT_ID/SECRET/REFRESH_TOKEN)"
        )
    if status == 404:
        return head + f"wrong fileId or not shared with {who}"
    return head + "unexpected Drive API error"


# --------------------------------------------------------------------------
# Network (replaced in tests)
# --------------------------------------------------------------------------
def get_access_token(env: Mapping[str, str]) -> str:
    """Get an access token (SA with drive scope; otherwise an OAuth refresh token)."""
    try:
        from google.auth.transport.requests import Request
    except ImportError as exc:
        raise DriveError(
            "missing google-auth: pip install -r scripts/requirements-drive.txt"
        ) from exc
    try:
        if env.get("GDRIVE_SA_JSON"):
            from google.oauth2 import service_account

            creds = service_account.Credentials.from_service_account_info(
                json.loads(env["GDRIVE_SA_JSON"]), scopes=[SCOPE]
            )
        else:
            from google.oauth2.credentials import Credentials

            creds = Credentials(
                token=None,
                refresh_token=env["GDRIVE_OAUTH_REFRESH_TOKEN"],
                client_id=env["GDRIVE_OAUTH_CLIENT_ID"],
                client_secret=env["GDRIVE_OAUTH_CLIENT_SECRET"],
                token_uri=TOKEN_URI,
                scopes=[SCOPE],
            )
        creds.refresh(Request())
    except DriveError:
        raise
    except Exception as exc:  # do not include str(exc): it could carry credential data
        raise DriveError(
            f"Drive: authentication failed ({type(exc).__name__}); "
            "check GDRIVE_SA_JSON / GDRIVE_OAUTH_*"
        ) from None
    return creds.token


def http_request(method: str, url: str, headers: dict, data: bytes | None = None):
    try:
        import requests
    except ImportError as exc:
        raise DriveError(
            "missing requests: pip install -r scripts/requirements-drive.txt"
        ) from exc
    return requests.request(method, url, headers=headers, data=data, timeout=120)


def _body(resp) -> Any:
    try:
        return resp.json()
    except ValueError:
        return {}


# --------------------------------------------------------------------------
# Operations
# --------------------------------------------------------------------------
def check(env: Mapping[str, str]) -> int:
    file_id = env.get("GDRIVE_FILE_ID")
    if not file_id or not (env.get("GDRIVE_SA_JSON") or has_oauth(env)):
        print("ERROR --check requires GDRIVE_FILE_ID and GDRIVE_SA_JSON (or GDRIVE_OAUTH_*)")
        return 1
    email = sa_client_email(env)
    token = get_access_token(env)
    resp = http_request("GET", CHECK_URL.format(id=file_id), {"Authorization": f"Bearer {token}"})
    body = _body(resp)
    if not 200 <= resp.status_code < 300:
        print("ERROR " + map_error(resp.status_code, error_reason(body), email))
        return 1
    name = body.get("name")
    can_edit = bool(body.get("capabilities", {}).get("canEdit"))
    print(f"Drive: file '{name}' (id {body.get('id')}), canEdit={str(can_edit).lower()}")
    if not can_edit:
        print("ERROR " + map_error(403, "insufficientFilePermissions", email))
        return 1
    if name != EXPECTED_NAME:
        print(f"WARNING the name is '{name}', expected '{EXPECTED_NAME}'")
    return 0


def upload(env: Mapping[str, str], pdf: Path) -> int:
    reason = skip_reason(env)
    if reason:
        print(reason)
        return 0
    if not pdf.is_file():
        print(f"ERROR PDF not found: {pdf.as_posix()}")
        return 1
    email = sa_client_email(env)
    token = get_access_token(env)
    headers = {"Authorization": f"Bearer {token}", "Content-Type": "application/pdf"}
    # media-only: no metadata body, so Drive keeps the id and name
    resp = http_request(
        "PATCH", UPLOAD_URL.format(id=env["GDRIVE_FILE_ID"]), headers, pdf.read_bytes()
    )
    body = _body(resp)
    if not 200 <= resp.status_code < 300:
        print("ERROR " + map_error(resp.status_code, error_reason(body), email))
        return 1
    name = body.get("name")
    print(f"Drive: updated '{name}' (id {body.get('id')}, modified {body.get('modifiedTime')})")
    if name != EXPECTED_NAME:
        print(f"WARNING the name is '{name}', expected '{EXPECTED_NAME}'")
    return 0


def main(argv: list[str] | None = None, env: Mapping[str, str] | None = None) -> int:
    p = argparse.ArgumentParser(description="Update the notebook PDF in Google Drive")
    p.add_argument("--file", default="build/testigos-de-turing-teambook.pdf", help="PDF to upload")
    p.add_argument("--check", action="store_true", help="only verify access (no upload)")
    args = p.parse_args(argv)
    env = os.environ if env is None else env
    try:
        if args.check:
            return check(env)
        return upload(env, Path(args.file))
    except DriveError as exc:
        print(f"ERROR {exc}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
