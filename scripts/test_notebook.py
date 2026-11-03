import contextlib
import io
import shutil
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import notebook as nb  # noqa: E402

REAL_ROOT = Path(__file__).resolve().parent.parent
HAS_GXX = shutil.which("g++") is not None

GOOD_SNIPPET = """/**
 * Uso: foo(x)
 * Complejidad: O(1)
 * Status: ok
 */
int foo(int x) { return x; }
"""


class Fixture(unittest.TestCase):
    """Temp repo root with the real template and helpers."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.root = Path(self._tmp.name)
        (self.root / "template").mkdir()
        shutil.copy(REAL_ROOT / "template" / "template.cpp", self.root / "template" / "template.cpp")

    def write(self, rel, text):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p

    def manifest(self, body):
        return self.write("contents.txt", body)

    def run_check(self):
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            code = nb.check(self.root, jobs=2)
        return code, buf.getvalue()

    def parse(self, body):
        return nb.parse_manifest(self.manifest(body))


class ParseManifestTests(Fixture):
    def test_basic_sections_and_entries(self):
        secs, errs = self.parse(
            "[Plantilla]\ntemplate/template.cpp | Plantilla\n\n[DP]\nsrc/dp/lcs.cpp | LCS\n"
        )
        self.assertEqual(errs, [])
        self.assertEqual([s.title for s in secs], ["Plantilla", "DP"])
        self.assertEqual(secs[1].items[0].path, "src/dp/lcs.cpp")
        self.assertEqual(secs[1].items[0].title, "LCS")
        self.assertEqual(secs[1].items[0].line, 5)

    def test_comment_suffix_and_hash_in_title_S2_5(self):
        secs, errs = self.parse("[M]\nsrc/math/c.cpp | Cuenta C# y A_B  # nota\n")
        self.assertEqual(errs, [])
        self.assertEqual(secs[0].items[0].title, "Cuenta C# y A_B")

    def test_comment_and_blank_lines_ignored_S2_9(self):
        secs, errs = self.parse("# comentario\n\n   # otro\n[A]\n\nsrc/a/b.cpp | T\n")
        self.assertEqual(errs, [])
        self.assertEqual(len(secs[0].items), 1)

    def test_accents_and_enie_S2_8(self):
        secs, errs = self.parse("[A]\nsrc/a/b.cpp | Arbol de expansion minima (ñandú)\n")
        self.assertEqual(errs, [])
        self.assertEqual(secs[0].items[0].title, "Arbol de expansion minima (ñandú)")

    def test_special_chars_in_title(self):
        secs, errs = self.parse("[A]\nsrc/a/b.cpp | A_B & 100% $x\n")
        self.assertEqual(errs, [])
        self.assertEqual(secs[0].items[0].title, "A_B & 100% $x")

    def test_tab_separator_rejected_S2_6(self):
        _, errs = self.parse("[A]\nsrc/a/b.cpp\tTitulo\n")
        self.assertEqual(len(errs), 1)
        self.assertTrue(errs[0].startswith("contents.txt:2:"), errs[0])
        self.assertIn("TAB", errs[0])

    def test_bad_path_regex_rejected(self):
        for bad in ("src/Dp/Lcs.cpp", "src/max flow (v2).cpp", "lib/a.cpp", "src/a-b.cpp", "src/a.h", "../src/a.cpp"):
            _, errs = self.parse(f"[A]\n{bad} | T\n")
            self.assertEqual(len(errs), 1, bad)
            self.assertIn("contents.txt:2:", errs[0])

    def test_empty_title_rejected(self):
        _, errs = self.parse("[A]\nsrc/a/b.cpp | \n")
        self.assertEqual(len(errs), 1)
        _, errs = self.parse("[A]\nsrc/a/b.cpp |   \n")
        self.assertEqual(len(errs), 1)
        _, errs = self.parse("[]\n")
        self.assertEqual(len(errs), 1)

    def test_missing_separator_rejected(self):
        _, errs = self.parse("[A]\nsrc/a/b.cpp Titulo\n")
        self.assertEqual(len(errs), 1)

    def test_non_latin1_in_title_rejected_S2_7(self):
        _, errs = self.parse("[A]\nsrc/a/b.cpp | Guion ‐ raro\n")
        self.assertEqual(len(errs), 1)
        self.assertIn("U+2010", errs[0])
        _, errs = self.parse("[A]\nsrc/a/b.cpp | Emoji \U0001F600\n")
        self.assertIn("U+1F600", errs[0])

    def test_entry_outside_section_rejected(self):
        _, errs = self.parse("src/a/b.cpp | T\n")
        self.assertEqual(len(errs), 1)

    def test_tex_entries(self):
        secs, errs = self.parse("[T]\n!tex tex/theory/euclid.tex | Euclides\n!tex tex/theory/x.tex\n")
        self.assertEqual(errs, [])
        self.assertEqual([i.kind for i in secs[0].items], ["tex", "tex"])
        self.assertEqual(secs[0].items[1].title, "")
        _, errs = self.parse("[T]\n!tex src/x.tex\n")
        self.assertEqual(len(errs), 1)

    def test_crlf_tolerated(self):
        secs, errs = self.parse("[A]\r\nsrc/a/b.cpp | T\r\n")
        self.assertEqual(errs, [])
        self.assertEqual(secs[0].items[0].title, "T")


class HeaderTests(unittest.TestCase):
    def test_parse_fields(self):
        meta = nb.parse_header(GOOD_SNIPPET)
        self.assertTrue(meta.has_header)
        self.assertEqual(meta.fields["Status"], "ok")
        self.assertEqual(meta.lines["Status"], 4)
        self.assertEqual(nb.header_diagnostics("src/x.cpp", meta), ([], []))

    def test_missing_status_is_error_S1_3(self):
        meta = nb.parse_header("/**\n * Uso: x\n * Complejidad: O(1)\n */\nint a;\n")
        errs, _ = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(len(errs), 1)
        self.assertIn("Status", errs[0])

    def test_invalid_status_is_error_S1_3(self):
        meta = nb.parse_header("/**\n * Uso: x\n * Complejidad: O(1)\n * Status: maybe\n */\n")
        errs, _ = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(len(errs), 1)
        self.assertIn("maybe", errs[0])
        self.assertIn("src/x.cpp:4:", errs[0])

    def test_no_header_is_error(self):
        meta = nb.parse_header("int a;\n")
        self.assertFalse(meta.has_header)
        errs, _ = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(len(errs), 1)

    def test_missing_complexity_is_warning_S1_4(self):
        meta = nb.parse_header("/**\n * Uso: x\n * Status: ok\n */\n")
        errs, warns = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(errs, [])
        self.assertEqual(len(warns), 1)
        self.assertIn("Complejidad", warns[0])

    def test_only_first_block_counts(self):
        meta = nb.parse_header("/**\n * Status: ok\n */\n/**\n * Status: broken\n */\n")
        self.assertEqual(meta.fields["Status"], "ok")


class LintTests(unittest.TestCase):
    def test_include_using_main_rejected_S1_1(self):
        for body in (
            "#include <bits/stdc++.h>\nint a;\n",
            "  #  include <vector>\n",
            "using namespace std;\n",
            "int main() { return 0; }\n",
            "int main (int argc, char** argv) {}\n",
        ):
            errs = nb.lint_snippet("src/x.cpp", body)
            self.assertTrue(errs, body)
            self.assertTrue(errs[0].startswith("src/x.cpp:"), errs[0])

    def test_line_number_reported(self):
        errs = nb.lint_snippet("src/x.cpp", "int a;\nint b;\nint main(){}\n")
        self.assertTrue(errs[0].startswith("src/x.cpp:3:"))

    def test_clean_snippet_and_comments_ok(self):
        body = "// main( es un demo, #include aparte\n/* using namespace std */\nint domain(int x){return x;}\nint a=0;\n"
        self.assertEqual(nb.lint_snippet("src/x.cpp", body), [])

    def test_latin1(self):
        self.assertEqual(nb.latin1_diagnostics("src/x.cpp", "// ñandú áé\n"), [])
        d = nb.latin1_diagnostics("src/x.cpp", "int a;\n// guion ‐ aqui\n")
        self.assertEqual(len(d), 1)
        self.assertIn("src/x.cpp:2:", d[0])
        self.assertIn("U+2010", d[0])


class ValidateTests(Fixture):
    def validate(self, manifest_body):
        secs, perrs = nb.parse_manifest(self.manifest(manifest_body))
        self.assertEqual(perrs, [])
        return nb.validate(self.root, secs)

    def test_clean(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        r = self.validate("[Plantilla]\ntemplate/template.cpp | Plantilla\n[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual((r.errors, r.warnings, r.infos), ([], [], []))

    def test_missing_file_S2_1(self):
        r = self.validate("[A]\nsrc/graphs/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("contents.txt:2:", r.errors[0])
        self.assertIn("src/graphs/foo.cpp", r.errors[0])

    def test_orphan_S2_2(self):
        self.write("src/math/extra.cpp", GOOD_SNIPPET)
        r = self.validate("[Plantilla]\ntemplate/template.cpp | Plantilla\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("src/math/extra.cpp", r.errors[0])
        self.assertIn("huerfano", r.errors[0])

    def test_quarantine_is_info_S2_3(self):
        self.write("src/graphs/flow/dinic.cpp", GOOD_SNIPPET.replace("Status: ok", "Status: broken"))
        r = self.validate("[Plantilla]\ntemplate/template.cpp | Plantilla\n")
        self.assertEqual(r.errors, [])
        self.assertEqual(len(r.infos), 1)
        self.assertIn("cuarentena", r.infos[0])
        self.assertIn("src/graphs/flow/dinic.cpp", r.infos[0])

    def test_broken_listed_is_error_S2_4(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET.replace("Status: ok", "Status: broken"))
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("broken", r.errors[0])

    def test_duplicate_entry(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n[B]\nsrc/a/foo.cpp | Foo otra vez\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("contents.txt:4:", r.errors[0])
        self.assertIn("duplicad", r.errors[0])

    def test_invalid_names_on_disk_S1_2(self):
        self.write("src/graphs/Max Flow (v2).cpp", GOOD_SNIPPET)
        self.write("src/dp/Knapsack.cpp", GOOD_SNIPPET)
        r = self.validate("[Plantilla]\ntemplate/template.cpp | Plantilla\n")
        self.assertEqual(len(r.errors), 2)
        self.assertTrue(all("nombre invalido" in e for e in r.errors))

    def test_lint_in_listed_snippet_S1_1(self):
        self.write("src/a/foo.cpp", "/**\n * Uso: x\n * Complejidad: O(1)\n * Status: ok\n */\n#include <vector>\nint main(){}\n")
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 2)
        self.assertIn("src/a/foo.cpp:6:", r.errors[0])
        self.assertIn("src/a/foo.cpp:7:", r.errors[1])

    def test_non_latin1_in_snippet_S2_7(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET.replace("foo(x)", "foo‐x"))
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("U+2010", r.errors[0])

    def test_missing_status_in_snippet_S1_3(self):
        self.write("src/a/foo.cpp", "/**\n * Uso: x\n * Complejidad: O(1)\n */\nint foo(){return 1;}\n")
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("Status", r.errors[0])

    def test_missing_complexity_warns_S1_4(self):
        self.write("src/a/foo.cpp", "/**\n * Uso: x\n * Status: ok\n */\nint foo(){return 1;}\n")
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(r.errors, [])
        self.assertEqual(len(r.warnings), 1)

    def test_tex_entry_must_exist(self):
        r = self.validate("[T]\n!tex tex/theory/x.tex | X\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("tex/theory/x.tex", r.errors[0])
        self.write("tex/theory/x.tex", "hola\n")
        r = self.validate("[T]\n!tex tex/theory/x.tex | X\n")
        self.assertEqual(r.errors, [])

    def test_discover_snippets(self):
        self.write("src/b/z.cpp", GOOD_SNIPPET)
        self.write("src/a/y.cpp", GOOD_SNIPPET)
        self.write("tests/demo/a/y.cpp", "int main(){}\n")
        self.assertEqual(nb.discover_snippets(self.root), ["src/a/y.cpp", "src/b/z.cpp"])


class CheckCommandTests(Fixture):
    BASE = "[Plantilla]\ntemplate/template.cpp | Plantilla\n[A]\n"

    def test_missing_manifest(self):
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("contents.txt", out)

    def test_no_gxx_exit_2_S3_6(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        with mock.patch.object(nb.shutil, "which", return_value=None):
            code, out = self.run_check()
        self.assertEqual(code, 2)
        self.assertIn("g++ no encontrado", out)

    def test_lints_run_without_gxx(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET + "int main(){}\n")
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        with mock.patch.object(nb.shutil, "which", return_value=None):
            code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("main(", out)

    @unittest.skipUnless(HAS_GXX, "g++ no disponible")
    def test_clean_tree_ok_S3_1_S3_4(self):
        # uses ll/vi defined only in template
        self.write("src/a/foo.cpp", GOOD_SNIPPET + "ll bar(vi v) { return sz(v); }\n")
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.assertIn("1 ok / 0 fallidos", out)

    @unittest.skipUnless(HAS_GXX, "g++ no disponible")
    def test_syntax_error_reported_others_continue_S3_2(self):
        self.write("src/a/good.cpp", GOOD_SNIPPET)
        self.write("src/a/bad.cpp", GOOD_SNIPPET.replace("return x;", "return x"))
        self.manifest(self.BASE + "src/a/bad.cpp | Bad\nsrc/a/good.cpp | Good\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertRegex(out, r"src/a/bad\.cpp:\d+:\d+")
        self.assertIn("1 ok / 1 fallidos", out)

    @unittest.skipUnless(HAS_GXX, "g++ no disponible")
    def test_warning_not_fatal_S3_3(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET.replace("return x;", "int unused; return x;"))
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.assertIn("unused", out)

    @unittest.skipUnless(HAS_GXX, "g++ no disponible")
    def test_conflicting_typedef_fails_identical_passes_S3_5(self):
        self.write("src/a/same.cpp", GOOD_SNIPPET + "typedef long long ll;\n")
        self.manifest(self.BASE + "src/a/same.cpp | Same\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.write("src/a/same.cpp", GOOD_SNIPPET + "typedef int ll;\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("src/a/same.cpp", out)

    @unittest.skipUnless(HAS_GXX, "g++ no disponible")
    def test_broken_template_aborts_S3_7(self):
        self.write("template/template.cpp", "int broken(\n")
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("template/template.cpp", out)
        self.assertNotIn("Resumen compilacion", out)

    @unittest.skipUnless(HAS_GXX, "g++ no disponible")
    def test_quarantine_exit_0_broken_listed_exit_1(self):
        self.write("src/a/ok.cpp", GOOD_SNIPPET)
        self.write("src/a/q.cpp", "/**\n * Uso: x\n * Complejidad: O(1)\n * Status: broken\n */\nthis does not compile\n")
        self.manifest(self.BASE + "src/a/ok.cpp | Ok\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.assertIn("INFO cuarentena: src/a/q.cpp", out)
        self.manifest(self.BASE + "src/a/ok.cpp | Ok\nsrc/a/q.cpp | Q\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)

    def test_cli_usage_error_exit_2(self):
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(nb.main([]), 2)


if __name__ == "__main__":
    unittest.main()
