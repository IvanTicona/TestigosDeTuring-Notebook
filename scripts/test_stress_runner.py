import contextlib
import io
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import notebook as nb  # noqa: E402

REAL_ROOT = Path(__file__).resolve().parent.parent
HAS_GXX = shutil.which("g++") is not None


def run_quiet(*args, **kwargs):
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        code = nb.run_stress(*args, **kwargs)
    return code, buf.getvalue()


class StressFixture(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.root = Path(self._tmp.name)

    def write(self, rel, text=""):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p


def cp(cmd, code=0, out="", err=""):
    return subprocess.CompletedProcess(cmd, code, out, err)


class Discovery(StressFixture):
    def test_sorted_cpp_only_and_excludes_common(self):
        self.write("tests/stress/common.h")
        self.write("tests/stress/dp/b.cpp")
        self.write("tests/stress/dp/a.cpp")
        self.write("tests/stress/strings/z.cpp")
        self.write("tests/stress/notes.txt")
        self.assertEqual(
            nb.discover_stress(self.root),
            ["tests/stress/dp/a.cpp", "tests/stress/dp/b.cpp", "tests/stress/strings/z.cpp"],
        )

    def test_missing_dir_is_empty(self):
        self.assertEqual(nb.discover_stress(self.root), [])

    def test_filter_matches_any_substring(self):
        rels = ["tests/stress/dp/lcs.cpp", "tests/stress/dp/edit_distance.cpp", "tests/stress/strings/kmp.cpp"]
        self.assertEqual(nb.filter_stress(rels, ["lcs", "kmp"]), [rels[0], rels[2]])
        self.assertEqual(nb.filter_stress(rels, []), rels)
        self.assertEqual(nb.filter_stress(rels, ["nomatch"]), [])


class StressCmd(unittest.TestCase):
    def test_base_flags(self):
        root = Path("/repo")
        cmd = nb.stress_cmd("g++", root, "tests/stress/dp/lcs.cpp", "/out/lcs", False)
        self.assertEqual(cmd[0], "g++")
        for flag in ("-std=gnu++17", "-O2", "-Wall", "-Wextra"):
            self.assertIn(flag, cmd)
        self.assertTrue(any(a.startswith("-I") for a in cmd))
        self.assertNotIn("-fsanitize=address,undefined", cmd)
        self.assertEqual(cmd[cmd.index("-o") + 1], "/out/lcs")
        self.assertTrue(cmd[-1].endswith("tests/stress/dp/lcs.cpp") or "tests/stress/dp/lcs.cpp" in cmd)

    def test_repo_root_define(self):
        root = Path("/repo")
        cmd = nb.stress_cmd("g++", root, "tests/stress/a.cpp", "/out/a", False)
        self.assertIn(f'-DNB_ROOT="{root.resolve().as_posix()}"', cmd)

    def test_sanitize_flags(self):
        cmd = nb.stress_cmd("g++", Path("/repo"), "tests/stress/a.cpp", "/out/a", True)
        for flag in ("-fsanitize=address,undefined", "-fno-sanitize-recover=all",
                     "-fno-omit-frame-pointer", "-g", "-D_GLIBCXX_DEBUG"):
            self.assertIn(flag, cmd)


class RunStressMocked(StressFixture):
    """subprocess.run / shutil.which mocked: no real g++ needed."""

    def setUp(self):
        super().setUp()
        self.write("tests/stress/dp/a.cpp")
        self.write("tests/stress/dp/b.cpp")
        self.calls = []
        self.compile_fail = set()
        self.run_fail = set()
        self.run_timeout = set()
        self.machine = "x86_64-linux-gnu"
        p1 = mock.patch.object(nb.shutil, "which", return_value="g++")
        p2 = mock.patch.object(nb.subprocess, "run", side_effect=self._fake_run)
        p1.start(); p2.start()
        self.addCleanup(p1.stop); self.addCleanup(p2.stop)

    def _fake_run(self, cmd, **kw):
        self.calls.append((cmd, kw))
        if "-dumpmachine" in cmd:
            return cp(cmd, 0, self.machine + "\n")
        name = next((n for n in ("a", "b") if f"/{n}" in " ".join(cmd).replace("\\", "/")), "?")
        if "-o" in cmd:
            src = cmd[-1] if cmd[-1].endswith(".cpp") else ""
            for n in self.compile_fail:
                if f"/{n}.cpp" in src:
                    return cp(cmd, 1, "", "error: boom")
            return cp(cmd, 0)
        for n in self.run_timeout:
            if f"{n}" in Path(cmd[0]).stem:
                raise subprocess.TimeoutExpired(cmd, kw.get("timeout", 1))
        for n in self.run_fail:
            if f"{n}" in Path(cmd[0]).stem:
                return cp(cmd, 1, "CHECK failed seed=1\n", "")
        return cp(cmd, 0, "ok\n")

    def compiles(self):
        return [c for c, _ in self.calls if "-o" in c]

    def runs(self):
        return [(c, kw) for c, kw in self.calls if "-o" not in c and "-dumpmachine" not in c]

    def test_all_pass_exit_0_and_summary(self):
        code, out = run_quiet(self.root, [], 1, 20, False, None)
        self.assertEqual(code, 0)
        self.assertIn("PASS dp/a", out)
        self.assertIn("PASS dp/b", out)
        self.assertIn("Stress summary: 2 passed / 0 failed", out)

    def test_no_tests_no_filters_exit_0(self):
        shutil.rmtree(self.root / "tests")
        code, out = run_quiet(self.root, [], 1, 20, False, None)
        self.assertEqual(code, 0)
        self.assertIn("no stress tests found", out)
        self.assertEqual(self.calls, [])

    def test_filter_no_match_exit_2(self):
        code, out = run_quiet(self.root, ["nomatch"], 1, 20, False, None)
        self.assertEqual(code, 2)
        self.assertIn("nomatch", out)
        self.assertEqual(self.compiles(), [])

    def test_filter_selects_subset(self):
        code, out = run_quiet(self.root, ["dp/a"], 1, 20, False, None)
        self.assertEqual(code, 0)
        self.assertEqual(len(self.compiles()), 1)
        self.assertIn("Stress summary: 1 passed / 0 failed", out)

    def test_no_gxx_exit_2(self):
        with mock.patch.object(nb.shutil, "which", return_value=None):
            code, out = run_quiet(self.root, [], 1, 20, False, None)
        self.assertEqual(code, 2)
        self.assertIn("g++ not found", out)

    def test_compile_error_exit_1_others_still_run(self):
        self.compile_fail = {"a"}
        code, out = run_quiet(self.root, [], 1, 20, False, None)
        self.assertEqual(code, 1)
        self.assertIn("FAIL dp/a", out)
        self.assertIn("compile", out)
        self.assertIn("PASS dp/b", out)
        self.assertIn("Stress summary: 1 passed / 1 failed", out)

    def test_run_failure_exit_1_shows_output(self):
        self.run_fail = {"b"}
        code, out = run_quiet(self.root, [], 1, 20, False, None)
        self.assertEqual(code, 1)
        self.assertIn("FAIL dp/b", out)
        self.assertIn("CHECK failed seed=1", out)
        self.assertIn("PASS dp/a", out)

    def test_timeout_is_fail(self):
        self.run_timeout = {"a"}
        code, out = run_quiet(self.root, [], 1, 3, False, None)
        self.assertEqual(code, 1)
        self.assertIn("FAIL dp/a", out)
        self.assertIn("timeout", out)
        self.assertEqual(self.runs()[0][1]["timeout"], 3)

    def test_seed_sets_env(self):
        run_quiet(self.root, ["dp/a"], 1, 20, False, 7)
        env = self.runs()[0][1]["env"]
        self.assertEqual(env["SEED"], "7")

    def test_no_seed_leaves_env_alone(self):
        with mock.patch.dict(nb.os.environ, {"SEED": "99"}):
            run_quiet(self.root, ["dp/a"], 1, 20, False, None)
            env = self.runs()[0][1]["env"]
            self.assertEqual(env.get("SEED"), "99")

    def test_sanitize_adds_flags(self):
        code, _ = run_quiet(self.root, ["dp/a"], 1, 20, True, None)
        self.assertEqual(code, 0)
        self.assertIn("-fsanitize=address,undefined", self.compiles()[0])

    def test_sanitize_on_mingw_exit_2(self):
        self.machine = "x86_64-w64-mingw32"
        code, out = run_quiet(self.root, [], 1, 20, True, None)
        self.assertEqual(code, 2)
        self.assertIn("MinGW", out)
        self.assertEqual(self.compiles(), [])

    def test_sanitize_on_mingw_exit_2_even_without_tests(self):
        shutil.rmtree(self.root / "tests")
        self.machine = "x86_64-w64-mingw32"
        code, _ = run_quiet(self.root, [], 1, 20, True, None)
        self.assertEqual(code, 2)

    def test_mingw_without_sanitize_ok(self):
        self.machine = "x86_64-w64-mingw32"
        code, _ = run_quiet(self.root, [], 1, 20, False, None)
        self.assertEqual(code, 0)

    def test_run_cwd_is_output_dir(self):
        run_quiet(self.root, ["dp/a"], 1, 20, False, None)
        cwd = Path(self.runs()[0][1]["cwd"]).as_posix()
        self.assertTrue(cwd.endswith("build/stress/dp"), cwd)


class CliWiring(unittest.TestCase):
    def test_parser_defaults_and_flags(self):
        a = nb.build_parser().parse_args(["test"])
        self.assertEqual((a.filters, a.jobs, a.timeout, a.sanitize, a.seed), ([], None, 20.0, False, None))
        a = nb.build_parser().parse_args(
            ["test", "dp", "lcs", "--jobs", "2", "--timeout", "5", "--sanitize", "--seed", "7"]
        )
        self.assertEqual((a.filters, a.jobs, a.timeout, a.sanitize, a.seed), (["dp", "lcs"], 2, 5.0, True, 7))

    def test_main_dispatch(self):
        with mock.patch.object(nb, "run_stress", return_value=0) as m:
            self.assertEqual(nb.main(["test", "dp", "--seed", "3"]), 0)
        args = m.call_args[0]
        self.assertEqual(args[1], ["dp"])
        self.assertEqual(args[5], 3)


PASS_TEST = """#include "tests/stress/template_lib.h"
#include "tests/stress/common.h"
int main() {
    for (int it = 0; it < 100; it++) {
        stress::iter = it;
        ll x = stress::rnd(1, 10);
        CHECK(x >= 1 && x <= 10, "x=" + to_string(x));
    }
    return 0;
}
"""

FAIL_TEST = """#include "tests/stress/template_lib.h"
#include "tests/stress/common.h"
int main() {
    for (int it = 0; it < 100; it++) {
        stress::iter = it;
        ll x = stress::rnd(1, 10);
        CHECK(x < 0, "x=" + to_string(x));
    }
    return 0;
}
"""

HANG_TEST = """#include "tests/stress/template_lib.h"
#include "tests/stress/common.h"
int main() { volatile int x = 0; for (;;) x = x + 1; }
"""

STDIN_TEST = """#include "tests/stress/template_lib.h"
#include "tests/stress/common.h"
int main() {
    stress::feed_stdin("3 4\\n");
    int a, b;
    CHECK(scanf("%d %d", &a, &b) == 2 && a == 3 && b == 4, "stdin");
    {
        stress::Capture cap;
        cout << "hello";
        CHECK(cap.str() == "hello", "capture");
    }
    return 0;
}
"""


@unittest.skipUnless(HAS_GXX, "g++ not available")
class RealGxx(StressFixture):
    def setUp(self):
        super().setUp()
        self.write("template/template.cpp", (REAL_ROOT / "template" / "template.cpp").read_text(encoding="utf-8"))
        self.write("tests/stress/common.h", (REAL_ROOT / "tests" / "stress" / "common.h").read_text(encoding="utf-8"))
        self.write("tests/stress/template_lib.h", (REAL_ROOT / "tests" / "stress" / "template_lib.h").read_text(encoding="utf-8"))

    def test_pass_and_fail_end_to_end(self):
        self.write("tests/stress/good.cpp", PASS_TEST)
        self.write("tests/stress/bad.cpp", FAIL_TEST)
        code, out = run_quiet(self.root, [], 2, 30, False, None)
        self.assertEqual(code, 1, out)
        self.assertIn("PASS good", out)
        self.assertIn("FAIL bad", out)
        self.assertIn("seed", out)
        self.assertIn("iteration", out)
        self.assertIn("x=", out)
        self.assertIn("Stress summary: 1 passed / 1 failed", out)

    def test_all_pass_exit_0(self):
        self.write("tests/stress/good.cpp", PASS_TEST)
        code, out = run_quiet(self.root, [], 1, 30, False, None)
        self.assertEqual(code, 0, out)
        self.assertIn("Stress summary: 1 passed / 0 failed", out)

    def test_seed_is_reproducible(self):
        self.write("tests/stress/bad.cpp", FAIL_TEST)
        _, o1 = run_quiet(self.root, [], 1, 30, False, 7)
        _, o2 = run_quiet(self.root, [], 1, 30, False, 7)
        _, o3 = run_quiet(self.root, [], 1, 30, False, 8)
        x1 = [l for l in o1.splitlines() if "x=" in l or "seed" in l]
        x2 = [l for l in o2.splitlines() if "x=" in l or "seed" in l]
        x3 = [l for l in o3.splitlines() if "seed" in l]
        self.assertTrue(x1)
        self.assertEqual(x1, x2)
        self.assertNotEqual([l for l in x1 if "seed" in l], x3)

    def test_timeout_end_to_end(self):
        self.write("tests/stress/hang.cpp", HANG_TEST)
        code, out = run_quiet(self.root, [], 1, 1, False, None)
        self.assertEqual(code, 1, out)
        self.assertIn("FAIL hang", out)
        self.assertIn("timeout", out)

    def test_feed_stdin_and_capture(self):
        self.write("tests/stress/io/helpers.cpp", STDIN_TEST)
        code, out = run_quiet(self.root, [], 1, 30, False, None)
        self.assertEqual(code, 0, out)

    def test_compile_error_exit_1(self):
        self.write("tests/stress/broken.cpp", "this is not c++\n")
        code, out = run_quiet(self.root, [], 1, 30, False, None)
        self.assertEqual(code, 1)
        self.assertIn("FAIL broken", out)


if __name__ == "__main__":
    unittest.main()
