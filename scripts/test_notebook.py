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
 * Usage: foo(x)
 * Complexity: O(1)
 * Status: untested
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
            "[Template]\ntemplate/template.cpp | Template\n\n[DP]\nsrc/dp/lcs.cpp | LCS\n"
        )
        self.assertEqual(errs, [])
        self.assertEqual([s.title for s in secs], ["Template", "DP"])
        self.assertEqual(secs[1].items[0].path, "src/dp/lcs.cpp")
        self.assertEqual(secs[1].items[0].title, "LCS")
        self.assertEqual(secs[1].items[0].line, 5)

    def test_comment_suffix_and_hash_in_title_S2_5(self):
        secs, errs = self.parse("[M]\nsrc/math/c.cpp | Count C# and A_B  # note\n")
        self.assertEqual(errs, [])
        self.assertEqual(secs[0].items[0].title, "Count C# and A_B")

    def test_comment_and_blank_lines_ignored_S2_9(self):
        secs, errs = self.parse("# comment\n\n   # other\n[A]\n\nsrc/a/b.cpp | T\n")
        self.assertEqual(errs, [])
        self.assertEqual(len(secs[0].items), 1)

    def test_accents_and_enie_S2_8(self):
        secs, errs = self.parse("[A]\nsrc/a/b.cpp | Minimum spanning tree (ñandú)\n")
        self.assertEqual(errs, [])
        self.assertEqual(secs[0].items[0].title, "Minimum spanning tree (ñandú)")

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
        _, errs = self.parse("[A]\nsrc/a/b.cpp Title\n")
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
        self.assertEqual(meta.fields["Status"], "untested")
        self.assertEqual(meta.lines["Status"], 4)
        self.assertEqual(nb.header_diagnostics("src/x.cpp", meta), ([], []))

    def test_missing_status_is_error_S1_3(self):
        meta = nb.parse_header("/**\n * Usage: x\n * Complexity: O(1)\n */\nint a;\n")
        errs, _ = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(len(errs), 1)
        self.assertIn("Status", errs[0])

    def test_invalid_status_is_error_S1_3(self):
        meta = nb.parse_header("/**\n * Usage: x\n * Complexity: O(1)\n * Status: maybe\n */\n")
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
        meta = nb.parse_header("/**\n * Usage: x\n * Status: ok\n */\n")
        errs, warns = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(errs, [])
        self.assertEqual(len(warns), 1)
        self.assertIn("Complexity", warns[0])

    def test_spanish_field_names_not_recognized(self):
        meta = nb.parse_header("/**\n * Uso: x\n * Complejidad: O(1)\n * Status: ok\n */\n")
        self.assertNotIn("Uso", meta.fields)
        self.assertNotIn("Complejidad", meta.fields)
        errs, warns = nb.header_diagnostics("src/x.cpp", meta)
        self.assertEqual(errs, [])
        self.assertEqual(len(warns), 2)

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
        body = "// main( is a demo, #include aside\n/* using namespace std */\nint domain(int x){return x;}\nint a=0;\n"
        self.assertEqual(nb.lint_snippet("src/x.cpp", body), [])

    def test_long_lines_warned(self):
        body = "int a;\n" + "x" * 80 + "\n" + "y" * 81 + "\n"
        w = nb.long_line_diagnostics("src/x.cpp", body)
        self.assertEqual(len(w), 1)
        self.assertTrue(w[0].startswith("src/x.cpp:3: WARNING"), w[0])
        self.assertIn("81 columns", w[0])

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
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual((r.errors, r.warnings), ([], []))
        self.assertEqual(r.infos, ["INFO untested: 1 snippet(s)"])

    def test_missing_file_S2_1(self):
        r = self.validate("[A]\nsrc/graphs/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("contents.txt:2:", r.errors[0])
        self.assertIn("src/graphs/foo.cpp", r.errors[0])

    def test_orphan_S2_2(self):
        self.write("src/math/extra.cpp", GOOD_SNIPPET)
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("src/math/extra.cpp", r.errors[0])
        self.assertIn("orphan", r.errors[0])

    def test_quarantine_is_info_S2_3(self):
        self.write("src/graphs/flow/dinic.cpp", GOOD_SNIPPET.replace("Status: untested", "Status: broken"))
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(r.errors, [])
        self.assertEqual(len(r.infos), 1)
        self.assertIn("quarantine", r.infos[0])
        self.assertIn("src/graphs/flow/dinic.cpp", r.infos[0])

    def test_broken_listed_is_error_S2_4(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET.replace("Status: untested", "Status: broken"))
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("broken", r.errors[0])

    def test_duplicate_entry(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n[B]\nsrc/a/foo.cpp | Foo otra vez\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("contents.txt:4:", r.errors[0])
        self.assertIn("duplicate", r.errors[0])

    def test_invalid_names_on_disk_S1_2(self):
        self.write("src/graphs/Max Flow (v2).cpp", GOOD_SNIPPET)
        self.write("src/dp/Knapsack.cpp", GOOD_SNIPPET)
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(len(r.errors), 2)
        self.assertTrue(all("invalid name" in e for e in r.errors))

    def test_lint_in_listed_snippet_S1_1(self):
        self.write("src/a/foo.cpp", "/**\n * Usage: x\n * Complexity: O(1)\n * Status: untested\n */\n#include <vector>\nint main(){}\n")
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
        self.write("src/a/foo.cpp", "/**\n * Usage: x\n * Complexity: O(1)\n */\nint foo(){return 1;}\n")
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("Status", r.errors[0])

    def test_missing_complexity_warns_S1_4(self):
        self.write("src/a/foo.cpp", "/**\n * Usage: x\n * Status: untested\n */\nint foo(){return 1;}\n")
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

    # --- status <-> stress test linkage (R2.8-R2.10) ---
    OK_SNIPPET = GOOD_SNIPPET.replace("Status: untested", "Status: ok")
    STRESS_STUB = "int main() {}\n"

    def test_ok_without_stress_test_is_error_R2_8(self):
        self.write("src/a/foo.cpp", self.OK_SNIPPET)
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("src/a/foo.cpp:4:", r.errors[0])
        self.assertIn("tests/stress/a/foo.cpp", r.errors[0])

    def test_ok_with_stress_test_is_clean_R2_8(self):
        self.write("src/a/foo.cpp", self.OK_SNIPPET)
        self.write("tests/stress/a/foo.cpp", self.STRESS_STUB)
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual((r.errors, r.warnings, r.infos), ([], [], []))

    def test_untested_without_stress_test_is_not_error_R2_8(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\n")
        self.assertEqual(r.errors, [])

    def test_untested_count_info_R2_9(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        self.write("src/a/bar.cpp", GOOD_SNIPPET)
        self.write("src/a/baz.cpp", self.OK_SNIPPET)
        self.write("tests/stress/a/baz.cpp", self.STRESS_STUB)
        r = self.validate("[A]\nsrc/a/foo.cpp | Foo\nsrc/a/bar.cpp | Bar\nsrc/a/baz.cpp | Baz\n")
        self.assertEqual(r.errors, [])
        self.assertEqual(r.infos, ["INFO untested: 2 snippet(s)"])

    def test_no_untested_info_when_zero_R2_9(self):
        self.write("src/a/baz.cpp", self.OK_SNIPPET)
        self.write("tests/stress/a/baz.cpp", self.STRESS_STUB)
        r = self.validate("[A]\nsrc/a/baz.cpp | Baz\n")
        self.assertEqual(r.infos, [])

    def test_quarantined_not_counted_as_untested_R2_9(self):
        self.write("src/a/dinic.cpp", GOOD_SNIPPET.replace("Status: untested", "Status: broken"))
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(len(r.infos), 1)
        self.assertIn("quarantine", r.infos[0])

    def test_orphan_stress_test_is_error_R2_10(self):
        self.write("tests/stress/a/bar.cpp", self.STRESS_STUB)
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(len(r.errors), 1)
        self.assertIn("tests/stress/a/bar.cpp", r.errors[0])
        self.assertIn("src/a/bar.cpp", r.errors[0])

    def test_stress_test_for_quarantined_snippet_allowed_R2_10(self):
        self.write("src/a/dinic.cpp", GOOD_SNIPPET.replace("Status: untested", "Status: broken"))
        self.write("tests/stress/a/dinic.cpp", self.STRESS_STUB)
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(r.errors, [])

    def test_common_header_and_non_cpp_ignored_R2_10(self):
        self.write("tests/stress/common.h", "// helpers\n")
        self.write("tests/stress/a/notes.txt", "x\n")
        r = self.validate("[Template]\ntemplate/template.cpp | Template\n")
        self.assertEqual(r.errors, [])

    def test_discover_snippets(self):
        self.write("src/b/z.cpp", GOOD_SNIPPET)
        self.write("src/a/y.cpp", GOOD_SNIPPET)
        self.write("tests/demo/a/y.cpp", "int main(){}\n")
        self.assertEqual(nb.discover_snippets(self.root), ["src/a/y.cpp", "src/b/z.cpp"])


class CheckCommandTests(Fixture):
    BASE = "[Template]\ntemplate/template.cpp | Template\n[A]\n"

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
        self.assertIn("g++ not found", out)

    def test_lints_run_without_gxx(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET + "int main(){}\n")
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        with mock.patch.object(nb.shutil, "which", return_value=None):
            code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("main(", out)

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_clean_tree_ok_S3_1_S3_4(self):
        # uses ll/vi defined only in template
        self.write("src/a/foo.cpp", GOOD_SNIPPET + "ll bar(vi v) { return sz(v); }\n")
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.assertIn("1 ok / 0 failed", out)

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_syntax_error_reported_others_continue_S3_2(self):
        self.write("src/a/good.cpp", GOOD_SNIPPET)
        self.write("src/a/bad.cpp", GOOD_SNIPPET.replace("return x;", "return x"))
        self.manifest(self.BASE + "src/a/bad.cpp | Bad\nsrc/a/good.cpp | Good\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertRegex(out, r"src/a/bad\.cpp:\d+:\d+")
        self.assertIn("1 ok / 1 failed", out)

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_warning_not_fatal_S3_3(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET.replace("return x;", "int unused; return x;"))
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.assertIn("unused", out)

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_conflicting_typedef_fails_identical_passes_S3_5(self):
        self.write("src/a/same.cpp", GOOD_SNIPPET + "typedef long long ll;\n")
        self.manifest(self.BASE + "src/a/same.cpp | Same\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.write("src/a/same.cpp", GOOD_SNIPPET + "typedef int ll;\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("src/a/same.cpp", out)

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_broken_template_aborts_S3_7(self):
        self.write("template/template.cpp", "int broken(\n")
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        self.manifest(self.BASE + "src/a/foo.cpp | Foo\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)
        self.assertIn("template/template.cpp", out)
        self.assertNotIn("Compilation summary", out)

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_quarantine_exit_0_broken_listed_exit_1(self):
        self.write("src/a/ok.cpp", GOOD_SNIPPET)
        self.write("src/a/q.cpp", "/**\n * Usage: x\n * Complexity: O(1)\n * Status: broken\n */\nthis does not compile\n")
        self.manifest(self.BASE + "src/a/ok.cpp | Ok\n")
        code, out = self.run_check()
        self.assertEqual(code, 0, out)
        self.assertIn("INFO quarantine: src/a/q.cpp", out)
        self.manifest(self.BASE + "src/a/ok.cpp | Ok\nsrc/a/q.cpp | Q\n")
        code, out = self.run_check()
        self.assertEqual(code, 1)

    def test_cli_usage_error_exit_2(self):
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(nb.main([]), 2)


class TexifyTests(unittest.TestCase):
    def test_special_chars_single_pass(self):
        self.assertEqual(nb.texify("\\"), r"\textbackslash{}")
        self.assertEqual(nb.texify("{}"), r"\{\}")
        self.assertEqual(nb.texify("$&#_%"), r"\$\&\#\_\%")
        self.assertEqual(nb.texify("^"), r"\textasciicircum{}")
        self.assertEqual(nb.texify("~"), r"\textasciitilde{}")

    def test_no_double_escape(self):
        # backslash replacement must not be re-escaped (braces of \textbackslash{})
        self.assertEqual(nb.texify("a\\b{"), r"a\textbackslash{}b\{")

    def test_title_union_find(self):
        self.assertEqual(nb.texify("Union_Find #1 & 100%"), r"Union\_Find \#1 \& 100\%")

    def test_csharp_and_accents_untouched(self):
        self.assertEqual(nb.texify("C#"), r"C\#")
        s = "Number theory: Euclid (ñandú) Achá"
        self.assertEqual(nb.texify(s), s)


class RenderTests(Fixture):
    BASE = "[Template]\ntemplate/template.cpp | Template\n"

    def sections(self, text):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        self.write("src/a/bar.cpp", GOOD_SNIPPET)
        self.write("tex/theory/euclid.tex", "hola\n")
        m = self.write("contents.txt", text)
        secs, errs = nb.parse_manifest(m)
        self.assertEqual(errs, [])
        return secs

    def test_order_and_template_first(self):
        secs = self.sections(
            self.BASE + "[One]\nsrc/a/foo.cpp | Foo\n[Two]\nsrc/a/bar.cpp | Bar\n"
        )
        out = nb.render_contents(secs)
        order = [out.index(x) for x in ("{Template}", "{One}", "{Two}")]
        self.assertEqual(order, sorted(order))
        self.assertIn(r"\lstinputlisting[style=cpp]{template/template.cpp}", out)
        self.assertIn(r"\lstinputlisting[style=cpp]{src/a/foo.cpp}", out)
        self.assertIn(r"\NBpath{template/template.cpp}", out)
        self.assertNotIn("caption", out)
        self.assertEqual(out.count(r"\clearpage"), 3)
        self.assertEqual(out.count(r"\begin{multicols*}{2}"), 3)
        self.assertEqual(out.count(r"\end{multicols*}"), 3)

    def test_quarantined_not_rendered(self):
        secs = self.sections(self.BASE + "[One]\nsrc/a/foo.cpp | Foo\n")
        self.write("src/a/q.cpp", GOOD_SNIPPET.replace("ok", "broken"))
        self.assertNotIn("q.cpp", nb.render_contents(secs))

    def test_tex_entry_becomes_input(self):
        secs = self.sections(
            self.BASE + "[Theory]\n!tex tex/theory/euclid.tex | Euclid (ñandú)\n"
        )
        out = nb.render_contents(secs)
        self.assertIn(r"\input{tex/theory/euclid.tex}", out)
        self.assertIn(r"\subsection{Euclid (ñandú)}", out)

    def test_titles_escaped(self):
        secs = self.sections(self.BASE + "[A_B & C#]\nsrc/a/foo.cpp | Union_Find#1 & 100%\n")
        out = nb.render_contents(secs)
        self.assertIn(r"\section{A\_B \& C\#}", out)
        self.assertIn(r"\subsection{Union\_Find\#1 \& 100\%}", out)
        self.assertNotIn("caption", out)
        self.assertIn(r"\NBpath{src/a/foo.cpp}", out)


class MetaAndTexCommandTests(Fixture):
    def test_english_date(self):
        import datetime
        meta = nb.render_meta(datetime.date(2026, 11, 3))
        self.assertIn(r"\newcommand{\NBdate}{November 3, 2026}", meta)
        self.assertIn("pdftitle", meta)

    def test_all_months(self):
        import datetime
        for m, name in enumerate(nb.MONTHS, 1):
            self.assertIn(f"{name} 1, 2026", nb.render_meta(datetime.date(2026, m, 1)))

    def test_tex_command_writes_files(self):
        self.write("src/a/foo.cpp", GOOD_SNIPPET)
        self.write("contents.txt", "[Template]\ntemplate/template.cpp | Template\n[A]\nsrc/a/foo.cpp | Foo\n")
        with contextlib.redirect_stdout(io.StringIO()):
            code = nb.tex(self.root)
        self.assertEqual(code, 0)
        self.assertTrue((self.root / "build" / "contents.tex").is_file())
        self.assertTrue((self.root / "build" / "meta.tex").is_file())

    def test_tex_command_bad_manifest_exit_1(self):
        self.write("contents.txt", "[A]\nsrc/a/foo.cpp\tFoo\n")
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(nb.tex(self.root), 1)

    def test_pdf_without_latexmk_exit_2(self):
        self.write("contents.txt", "[Template]\ntemplate/template.cpp | Template\n")
        out = io.StringIO()
        with mock.patch.object(nb.shutil, "which", return_value=None), contextlib.redirect_stdout(out):
            self.assertEqual(nb.pdf(self.root, "both"), 2)
        self.assertIn("latexmk not found", out.getvalue())

    def test_latexmk_command_themes(self):
        read = nb.latexmk_cmd("latexmk", "read")
        self.assertIn("-outdir=build/read", read)
        self.assertIn("-jobname=notebook-read", read)
        self.assertFalse(any("usepretex" in a for a in read))
        pr = nb.latexmk_cmd("latexmk", "print")
        self.assertIn("-outdir=build/print", pr)
        self.assertIn(nb.pretex_arg(), pr)
        self.assertEqual(pr[-1], "tex/notebook.tex")

    def test_pretex_arg_quoting_per_platform(self):
        self.assertEqual(nb.pretex_arg("win32"), r'-usepretex="\def\PRINTMODE{1}"')
        self.assertEqual(nb.pretex_arg("linux"), r"-usepretex=\def\PRINTMODE{1}")

    def test_pdf_parser_theme_choices(self):
        a = nb.build_parser().parse_args(["pdf"])
        self.assertEqual(a.theme, "both")
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(nb.main(["pdf", "--theme", "x"]), 2)


DEP_A = """/**
 * Usage: twice(x)
 * Complexity: O(1)
 * Status: untested
 */
int twice(int x) { return 2 * x; }
"""
DEP_B = """/**
 * Usage: quad(x)
 * Complexity: O(1)
 * Depends: src/a/dep_a.cpp
 * Status: untested
 */
int quad(int x) { return twice(twice(x)); }
"""


class DependsTests(Fixture):
    def validate(self, manifest_body):
        secs, perrs = nb.parse_manifest(self.manifest(manifest_body))
        self.assertEqual(perrs, [])
        return nb.validate(self.root, secs)

    def test_header_parses_depends(self):
        meta = nb.parse_header(DEP_B)
        self.assertEqual(meta.fields["Depends"], "src/a/dep_a.cpp")

    def test_repeated_depends_lines_accumulate(self):
        text = DEP_B.replace(" * Depends: src/a/dep_a.cpp\n",
                             " * Depends: src/a/dep_a.cpp\n * Depends: src/a/dep_x.cpp\n")
        meta = nb.parse_header(text)
        self.assertEqual(nb.parse_depends(meta.fields["Depends"]),
                         ["src/a/dep_a.cpp", "src/a/dep_x.cpp"])

    def test_depends_listed_earlier_is_clean(self):
        self.write("src/a/dep_a.cpp", DEP_A)
        self.write("src/a/dep_b.cpp", DEP_B)
        r = self.validate("[A]\nsrc/a/dep_a.cpp | A\nsrc/a/dep_b.cpp | B\n")
        self.assertEqual(r.errors, [])

    def test_depends_missing_file_is_error(self):
        self.write("src/a/dep_b.cpp", DEP_B)
        r = self.validate("[A]\nsrc/a/dep_b.cpp | B\n")
        self.assertTrue(any("depends on src/a/dep_a.cpp" in e for e in r.errors), r.errors)

    def test_depends_listed_after_is_error(self):
        self.write("src/a/dep_a.cpp", DEP_A)
        self.write("src/a/dep_b.cpp", DEP_B)
        r = self.validate("[A]\nsrc/a/dep_b.cpp | B\nsrc/a/dep_a.cpp | A\n")
        self.assertTrue(any("must be listed before" in e for e in r.errors), r.errors)

    def test_snippet_deps_transitive_order(self):
        self.write("src/a/dep_a.cpp", DEP_A)
        self.write("src/a/dep_b.cpp", DEP_B)
        dep_c = DEP_B.replace("dep_a.cpp", "dep_b.cpp").replace("quad", "oct")
        self.write("src/a/dep_c.cpp", dep_c)
        self.assertEqual(nb.snippet_deps(self.root, "src/a/dep_c.cpp"),
                         ["src/a/dep_a.cpp", "src/a/dep_b.cpp"])

    @unittest.skipUnless(HAS_GXX, "g++ not available")
    def test_compile_includes_dependencies(self):
        self.write("template/template.cpp", "#include <bits/stdc++.h>\nusing namespace std;\n")
        self.write("src/a/dep_a.cpp", DEP_A)
        self.write("src/a/dep_b.cpp", DEP_B)
        ok, out = nb.compile_one("g++", self.root, "src/a/dep_b.cpp")
        self.assertTrue(ok, out)
        self.write("src/a/dep_b.cpp", DEP_B.replace(" * Depends: src/a/dep_a.cpp\n", ""))
        ok, _ = nb.compile_one("g++", self.root, "src/a/dep_b.cpp")
        self.assertFalse(ok)


if __name__ == "__main__":
    unittest.main()
