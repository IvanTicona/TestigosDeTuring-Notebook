"""Tests de drive_upload.py con HTTP y token simulados (sin google-auth ni red)."""

from __future__ import annotations

import contextlib
import io
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import drive_upload as du  # noqa: E402

SECRET = "TOP-SECRET-PRIVATE-KEY"
SA = json.dumps({"client_email": "bot@proj.iam.gserviceaccount.com", "private_key": SECRET})
ENV = {"GDRIVE_FILE_ID": "FID123", "GDRIVE_SA_JSON": SA}


class FakeResp:
    def __init__(self, status: int, body: dict | None = None):
        self.status_code = status
        self._body = body

    def json(self):
        if self._body is None:
            raise ValueError("no json")
        return self._body


def err(status: int, reason: str) -> FakeResp:
    return FakeResp(status, {"error": {"errors": [{"reason": reason}], "message": SECRET}})


def run(argv, env, resp=None):
    """Ejecuta main con token y HTTP simulados; devuelve (exit, salida, mock_http)."""
    out = io.StringIO()
    with mock.patch.object(du, "get_access_token", return_value="tok") as _tok, \
         mock.patch.object(du, "http_request", return_value=resp) as http, \
         contextlib.redirect_stdout(out):
        code = du.main(argv, env)
    return code, out.getvalue(), http


class SkipTests(unittest.TestCase):
    def test_missing_everything_skips_with_exit_0(self):
        code, out, http = run([], {})
        self.assertEqual(code, 0)
        self.assertIn("Drive omitido: faltan secrets", out)
        http.assert_not_called()

    def test_missing_file_id_skips(self):
        code, out, _ = run([], {"GDRIVE_SA_JSON": SA})
        self.assertEqual(code, 0)
        self.assertIn("Drive omitido", out)

    def test_missing_credentials_skips(self):
        code, out, _ = run([], {"GDRIVE_FILE_ID": "x"})
        self.assertEqual(code, 0)
        self.assertIn("Drive omitido", out)

    def test_partial_oauth_trio_skips(self):
        env = {"GDRIVE_FILE_ID": "x", "GDRIVE_OAUTH_CLIENT_ID": "a", "GDRIVE_OAUTH_CLIENT_SECRET": "b"}
        code, out, _ = run([], env)
        self.assertEqual(code, 0)
        self.assertIn("Drive omitido", out)

    def test_full_oauth_trio_does_not_skip(self):
        env = {
            "GDRIVE_FILE_ID": "x", "GDRIVE_OAUTH_CLIENT_ID": "a",
            "GDRIVE_OAUTH_CLIENT_SECRET": "b", "GDRIVE_OAUTH_REFRESH_TOKEN": "c",
        }
        self.assertIsNone(du.skip_reason(env))

    def test_check_without_secrets_fails_not_skips(self):
        code, out, _ = run(["--check"], {})
        self.assertEqual(code, 1)
        self.assertNotIn("omitido", out)


class CheckTests(unittest.TestCase):
    def test_can_edit_true_exit_0_and_get_without_upload(self):
        resp = FakeResp(200, {"id": "FID123", "name": du.EXPECTED_NAME, "capabilities": {"canEdit": True}})
        code, out, http = run(["--check"], ENV, resp)
        self.assertEqual(code, 0)
        self.assertIn("canEdit=true", out)
        method, url = http.call_args.args[0], http.call_args.args[1]
        self.assertEqual(method, "GET")
        self.assertIn("capabilities/canEdit", url)
        self.assertNotIn("upload", url)

    def test_can_edit_false_exit_1_with_share_hint(self):
        resp = FakeResp(200, {"id": "FID123", "name": du.EXPECTED_NAME, "capabilities": {"canEdit": False}})
        code, out, _ = run(["--check"], ENV, resp)
        self.assertEqual(code, 1)
        self.assertIn("canEdit=false", out)
        self.assertIn("bot@proj.iam.gserviceaccount.com", out)
        self.assertIn("Editor", out)

    def test_check_404(self):
        code, out, _ = run(["--check"], ENV, err(404, "notFound"))
        self.assertEqual(code, 1)
        self.assertIn("fileId incorrecto", out)


class UploadTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.pdf = Path(tmp.name) / "x.pdf"
        self.pdf.write_bytes(b"%PDF-1.5 fake")

    def test_patch_success_is_media_only_and_keeps_name(self):
        resp = FakeResp(200, {"id": "FID123", "name": du.EXPECTED_NAME, "modifiedTime": "T"})
        code, out, http = run(["--file", str(self.pdf)], ENV, resp)
        self.assertEqual(code, 0)
        method, url, headers, data = http.call_args.args
        self.assertEqual(method, "PATCH")
        self.assertIn("/upload/drive/v3/files/FID123", url)
        self.assertIn("uploadType=media", url)
        self.assertIn("supportsAllDrives=true", url)
        self.assertEqual(headers["Content-Type"], "application/pdf")
        self.assertEqual(data, b"%PDF-1.5 fake")  # solo bytes del PDF, sin metadata JSON
        self.assertNotIn("WARNING", out)

    def test_name_mismatch_warns_but_succeeds(self):
        resp = FakeResp(200, {"id": "FID123", "name": "otro.pdf", "modifiedTime": "T"})
        code, out, _ = run(["--file", str(self.pdf)], ENV, resp)
        self.assertEqual(code, 0)
        self.assertIn("WARNING", out)

    def test_oauth_fallback_uploads(self):
        env = {
            "GDRIVE_FILE_ID": "FID123", "GDRIVE_OAUTH_CLIENT_ID": "a",
            "GDRIVE_OAUTH_CLIENT_SECRET": "b", "GDRIVE_OAUTH_REFRESH_TOKEN": "c",
        }
        resp = FakeResp(200, {"id": "FID123", "name": du.EXPECTED_NAME})
        code, _, http = run(["--file", str(self.pdf)], env, resp)
        self.assertEqual(code, 0)
        http.assert_called_once()

    def test_quota_exceeded_suggests_oauth_and_hides_secrets(self):
        code, out, _ = run(["--file", str(self.pdf)], ENV, err(403, "storageQuotaExceeded"))
        self.assertEqual(code, 1)
        self.assertIn("OAuth", out)
        self.assertIn("storageQuotaExceeded", out)
        self.assertNotIn(SECRET, out)

    def test_insufficient_permissions_names_client_email(self):
        code, out, _ = run(["--file", str(self.pdf)], ENV, err(403, "insufficientFilePermissions"))
        self.assertEqual(code, 1)
        self.assertIn("compartir el archivo con bot@proj.iam.gserviceaccount.com como Editor", out)

    def test_404_wrong_file_id_message(self):
        code, out, _ = run(["--file", str(self.pdf)], ENV, err(404, "notFound"))
        self.assertEqual(code, 1)
        self.assertIn("fileId incorrecto o no compartido con bot@proj.iam.gserviceaccount.com", out)
        self.assertNotIn(SECRET, out)

    def test_401_invalid_credentials(self):
        code, out, _ = run(["--file", str(self.pdf)], ENV, err(401, "authError"))
        self.assertEqual(code, 1)
        self.assertIn("credenciales invalidas", out)

    def test_non_json_error_body_does_not_crash(self):
        code, out, _ = run(["--file", str(self.pdf)], ENV, FakeResp(502, None))
        self.assertEqual(code, 1)
        self.assertIn("502", out)

    def test_missing_pdf_fails(self):
        code, out, http = run(["--file", str(self.pdf) + ".nope"], ENV, None)
        self.assertEqual(code, 1)
        http.assert_not_called()


class MapErrorTests(unittest.TestCase):
    def test_pure_mapping(self):
        self.assertIn("OAuth", du.map_error(403, "storageQuotaExceeded"))
        self.assertIn("a@b", du.map_error(403, "insufficientFilePermissions", "a@b"))
        self.assertIn("fileId incorrecto", du.map_error(404, "notFound", "a@b"))
        self.assertIn("credenciales", du.map_error(401, "x"))

    def test_error_reason_extraction(self):
        self.assertEqual(du.error_reason({"error": {"errors": [{"reason": "r"}]}}), "r")
        self.assertEqual(du.error_reason({"error": {"status": "S"}}), "S")
        self.assertEqual(du.error_reason({}), "desconocido")


class AuthTests(unittest.TestCase):
    def test_token_failure_does_not_leak_secret(self):
        # sin google-auth instalado -> DriveError con mensaje de instalacion; con el, fallo de auth
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            code = du.main(["--check"], ENV)
        self.assertEqual(code, 1)
        self.assertNotIn(SECRET, out.getvalue())


if __name__ == "__main__":
    unittest.main()
