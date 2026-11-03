#!/usr/bin/env python3
"""Sube el PDF del notebook a un archivo de Google Drive ya existente.

Actualiza el CONTENIDO del archivo (PATCH media-only) para conservar fileId,
nombre y enlaces compartidos. Nunca imprime secretos ni tokens.

Entorno:
  GDRIVE_FILE_ID                      id del archivo en Drive (obligatorio)
  GDRIVE_SA_JSON                      JSON de la service account (auth primaria)
  GDRIVE_OAUTH_CLIENT_ID / _SECRET / _REFRESH_TOKEN
                                      fallback OAuth si no hay GDRIVE_SA_JSON

Uso:
  python scripts/drive_upload.py --file build/testigos-de-turing-teambook.pdf
  python scripts/drive_upload.py --check     # solo verifica permisos (no sube)

Exit: 0 ok u omitido por falta de secretos (solo subida); 1 fallo.
google-auth y requests se importan de forma perezosa (ver requirements-drive.txt).
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Any, Mapping

EXPECTED_NAME = "testigos-de-turing-teambook.pdf"
SCOPE = "https://www.googleapis.com/auth/drive"  # drive.file no sirve: el archivo no lo creo la app
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
SKIP_MESSAGE = "Drive omitido: faltan secrets"


class DriveError(Exception):
    """Fallo de Drive con mensaje seguro (sin secretos)."""


# --------------------------------------------------------------------------
# Logica pura
# --------------------------------------------------------------------------
def has_oauth(env: Mapping[str, str]) -> bool:
    return all(env.get(k) for k in OAUTH_VARS)


def skip_reason(env: Mapping[str, str]) -> str | None:
    """Mensaje de omision si faltan secretos; None si se puede continuar."""
    if not env.get("GDRIVE_FILE_ID"):
        return f"{SKIP_MESSAGE} (GDRIVE_FILE_ID)"
    if not env.get("GDRIVE_SA_JSON") and not has_oauth(env):
        return f"{SKIP_MESSAGE} (GDRIVE_SA_JSON o GDRIVE_OAUTH_*)"
    return None


def sa_client_email(env: Mapping[str, str]) -> str | None:
    """client_email de la SA (unico dato de la SA que se puede mostrar)."""
    raw = env.get("GDRIVE_SA_JSON")
    if not raw:
        return None
    try:
        email = json.loads(raw).get("client_email")
    except (ValueError, AttributeError):
        return None
    return email if isinstance(email, str) else None


def error_reason(body: Any) -> str:
    """Extrae error.errors[0].reason de un cuerpo de error de la API."""
    try:
        return str(body["error"]["errors"][0]["reason"])
    except (KeyError, IndexError, TypeError):
        pass
    try:
        return str(body["error"]["status"])
    except (KeyError, TypeError):
        return "desconocido"


def map_error(status: int, reason: str, client_email: str | None = None) -> str:
    """Mensaje accionable para un HTTP no 2xx (funcion pura)."""
    who = client_email or "la cuenta usada"
    head = f"Drive HTTP {status} ({reason}): "
    if status == 401:
        return head + "credenciales invalidas o expiradas (revisar GDRIVE_SA_JSON / GDRIVE_OAUTH_*)"
    if status == 403 and reason == "insufficientFilePermissions":
        return head + f"compartir el archivo con {who} como Editor"
    if status == 403 and reason == "storageQuotaExceeded":
        return head + (
            "la service account no tiene cuota de almacenamiento; "
            "usar el fallback OAuth (GDRIVE_OAUTH_CLIENT_ID/SECRET/REFRESH_TOKEN)"
        )
    if status == 404:
        return head + f"fileId incorrecto o no compartido con {who}"
    return head + "error inesperado de la API de Drive"


# --------------------------------------------------------------------------
# Red (se sustituye en los tests)
# --------------------------------------------------------------------------
def get_access_token(env: Mapping[str, str]) -> str:
    """Obtiene un access token (SA con scope drive; si no, refresh token OAuth)."""
    try:
        from google.auth.transport.requests import Request
    except ImportError as exc:
        raise DriveError(
            "falta google-auth: pip install -r scripts/requirements-drive.txt"
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
    except Exception as exc:  # no incluir str(exc): podria traer datos de credenciales
        raise DriveError(
            f"Drive: no se pudo autenticar ({type(exc).__name__}); "
            "revisar GDRIVE_SA_JSON / GDRIVE_OAUTH_*"
        ) from None
    return creds.token


def http_request(method: str, url: str, headers: dict, data: bytes | None = None):
    try:
        import requests
    except ImportError as exc:
        raise DriveError(
            "falta requests: pip install -r scripts/requirements-drive.txt"
        ) from exc
    return requests.request(method, url, headers=headers, data=data, timeout=120)


def _body(resp) -> Any:
    try:
        return resp.json()
    except ValueError:
        return {}


# --------------------------------------------------------------------------
# Operaciones
# --------------------------------------------------------------------------
def check(env: Mapping[str, str]) -> int:
    file_id = env.get("GDRIVE_FILE_ID")
    if not file_id or not (env.get("GDRIVE_SA_JSON") or has_oauth(env)):
        print("ERROR --check requiere GDRIVE_FILE_ID y GDRIVE_SA_JSON (o GDRIVE_OAUTH_*)")
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
    print(f"Drive: archivo '{name}' (id {body.get('id')}), canEdit={str(can_edit).lower()}")
    if not can_edit:
        print("ERROR " + map_error(403, "insufficientFilePermissions", email))
        return 1
    if name != EXPECTED_NAME:
        print(f"WARNING el nombre es '{name}', se esperaba '{EXPECTED_NAME}'")
    return 0


def upload(env: Mapping[str, str], pdf: Path) -> int:
    reason = skip_reason(env)
    if reason:
        print(reason)
        return 0
    if not pdf.is_file():
        print(f"ERROR no existe el PDF: {pdf.as_posix()}")
        return 1
    email = sa_client_email(env)
    token = get_access_token(env)
    headers = {"Authorization": f"Bearer {token}", "Content-Type": "application/pdf"}
    # media-only: sin cuerpo de metadata, para que Drive conserve id y nombre
    resp = http_request(
        "PATCH", UPLOAD_URL.format(id=env["GDRIVE_FILE_ID"]), headers, pdf.read_bytes()
    )
    body = _body(resp)
    if not 200 <= resp.status_code < 300:
        print("ERROR " + map_error(resp.status_code, error_reason(body), email))
        return 1
    name = body.get("name")
    print(f"Drive: actualizado '{name}' (id {body.get('id')}, modificado {body.get('modifiedTime')})")
    if name != EXPECTED_NAME:
        print(f"WARNING el nombre es '{name}', se esperaba '{EXPECTED_NAME}'")
    return 0


def main(argv: list[str] | None = None, env: Mapping[str, str] | None = None) -> int:
    p = argparse.ArgumentParser(description="Actualiza el PDF del notebook en Google Drive")
    p.add_argument("--file", default="build/testigos-de-turing-teambook.pdf", help="PDF a subir")
    p.add_argument("--check", action="store_true", help="solo verificar acceso (no sube)")
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
