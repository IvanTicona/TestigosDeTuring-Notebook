#!/usr/bin/env python3
"""Testigos de Turing notebook tooling (stdlib only).

Usage: python scripts/notebook.py <check [--jobs N] | tex | pdf [--theme read|print|both]>

Exit codes: 0 OK; 1 validation/compilation failure; 2 usage or environment error (g++/latexmk missing).
"""
from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST_NAME = "contents.txt"
TEMPLATE_REL = "template/template.cpp"

CODE_PATH_RE = re.compile(r"^(src|template)/[a-z0-9_/]+\.cpp$")
TEX_PATH_RE = re.compile(r"^tex/[a-z0-9_/]+\.tex$")
SNIPPET_PATH_RE = re.compile(r"^src/[a-z0-9_/]+\.cpp$")
HEADER_FIELD_RE = re.compile(r"^\s*\*\s*(Usage|Complexity|Source|Tested|Note|Status):\s*(.+?)\s*$")
VALID_STATUS = ("ok", "untested", "broken")

GXX_FLAGS = ["-std=gnu++17", "-fsyntax-only", "-Wall", "-Wextra"]


# --------------------------------------------------------------------------
# Data model
# --------------------------------------------------------------------------
@dataclass
class Entry:
    kind: str  # "code" | "tex"
    path: str  # posix path relative to repo root
    title: str
    line: int


@dataclass
class Section:
    title: str
    line: int
    items: list[Entry] = field(default_factory=list)


@dataclass
class Meta:
    has_header: bool
    fields: dict[str, str]
    lines: dict[str, int]  # field -> 1-based line in the file
    header_line: int = 1


@dataclass
class Report:
    errors: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)
    infos: list[str] = field(default_factory=list)


# --------------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------------
def non_latin1(text: str) -> list[tuple[int, int, int]]:
    """Return (line, col, codepoint) of every char outside Latin-1."""
    out = []
    for lineno, line in enumerate(text.split("\n"), 1):
        for col, ch in enumerate(line, 1):
            if ord(ch) > 0xFF:
                out.append((lineno, col, ord(ch)))
    return out


def _cp(cp: int) -> str:
    return f"U+{cp:04X}"


# --------------------------------------------------------------------------
# Manifest parser
# --------------------------------------------------------------------------
def strip_comment(line: str) -> str:
    """Remove a '#' comment: whole line starting with '#', or suffix ' #...'."""
    s = line.strip()
    if s.startswith("#"):
        return ""
    idx = s.find(" #")
    if idx != -1:
        s = s[:idx]
    return s.strip()


def parse_manifest(path: Path, label: str | None = None) -> tuple[list[Section], list[str]]:
    """Parse contents.txt. Returns (sections, errors); errors are 'label:<n>: msg'."""
    label = label or path.name
    errors: list[str] = []
    sections: list[Section] = []
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError as e:
        return [], [f"{label}:1: the manifest is not valid UTF-8 ({e.reason})"]

    def err(n: int, msg: str) -> None:
        errors.append(f"{label}:{n}: {msg}")

    for n, raw in enumerate(text.split("\n"), 1):
        raw = raw.rstrip("\r")
        line = strip_comment(raw)
        if not line:
            continue
        bad = non_latin1(line)
        if bad:
            _, col, cp = bad[0]
            err(n, f"character outside Latin-1 {_cp(cp)} (column {col})")
            continue
        if "\t" in line:
            err(n, "invalid format: TAB not allowed, the separator is ' | '")
            continue

        # Section header
        if line.startswith("["):
            if not line.endswith("]"):
                err(n, "section missing closing ']'")
                continue
            title = line[1:-1].strip()
            if not title:
                err(n, "empty section title")
                continue
            sections.append(Section(title, n))
            continue

        # Theory entry
        if line.startswith("!tex"):
            rest = line[4:]
            if not rest.startswith(" "):
                err(n, "invalid format: expected '!tex TEX_PATH [ | Title]'")
                continue
            rest = rest.strip()
            if " | " in rest:
                path_s, title = rest.split(" | ", 1)
                path_s, title = path_s.strip(), title.strip()
                if not title:
                    err(n, "empty title")
                    continue
            else:
                path_s, title = rest, ""
            if not TEX_PATH_RE.match(path_s):
                err(n, f"invalid path '{path_s}' (expected ^tex/[a-z0-9_/]+\\.tex$)")
                continue
            entry = Entry("tex", path_s, title, n)
        else:
            m = re.match(r"^(\S+) \| (.*)$", line)
            if not m:
                if line.endswith("|"):
                    err(n, "empty title")
                else:
                    err(n, "invalid format: expected 'PATH | Title' (separator ' | ')")
                continue
            path_s, title = m.group(1), m.group(2).strip()
            if not title:
                err(n, "empty title")
                continue
            if not CODE_PATH_RE.match(path_s):
                err(n, f"invalid path '{path_s}' (expected ^(src|template)/[a-z0-9_/]+\\.cpp$)")
                continue
            entry = Entry("code", path_s, title, n)

        if not sections:
            err(n, "entry outside a section ([Title])")
            continue
        sections[-1].items.append(entry)
    return sections, errors


# --------------------------------------------------------------------------
# Snippet header / lint
# --------------------------------------------------------------------------
def parse_header(text: str) -> Meta:
    """Parse the first /** ... */ block of a snippet."""
    m = re.search(r"/\*\*(.*?)\*/", text, re.S)
    if not m:
        return Meta(False, {}, {})
    start_line = text.count("\n", 0, m.start()) + 1
    fields: dict[str, str] = {}
    lines: dict[str, int] = {}
    for i, ln in enumerate(m.group(0).split("\n")):
        fm = HEADER_FIELD_RE.match(ln)
        if fm and fm.group(1) not in fields:
            fields[fm.group(1)] = fm.group(2)
            lines[fm.group(1)] = start_line + i
    return Meta(True, fields, lines, start_line)


def header_diagnostics(rel: str, meta: Meta) -> tuple[list[str], list[str]]:
    """(errors, warnings) for a snippet header."""
    errors, warnings = [], []
    status = meta.fields.get("Status")
    if status is None:
        errors.append(f"{rel}:{meta.header_line}: missing 'Status:' in header (ok|untested|broken)")
    elif status not in VALID_STATUS:
        errors.append(
            f"{rel}:{meta.lines['Status']}: invalid Status '{status}' (ok|untested|broken)"
        )
    for key in ("Usage", "Complexity"):
        if key not in meta.fields:
            warnings.append(f"{rel}:{meta.header_line}: WARNING missing '{key}:' in header")
    return errors, warnings


_COMMENT_OR_STRING = re.compile(
    r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|/\*.*?\*/|//[^\n]*', re.S
)


def _blank_comments(text: str) -> str:
    def repl(m: re.Match) -> str:
        s = m.group(0)
        if s.startswith("/*") or s.startswith("//"):
            return re.sub(r"[^\n]", " ", s)
        return s

    return _COMMENT_OR_STRING.sub(repl, text)


def lint_snippet(rel: str, text: str) -> list[str]:
    """Library-only lint: no #include, no 'using namespace std', no main(."""
    errors = []
    code = _blank_comments(text)
    rules = [
        (re.compile(r"^\s*#\s*include\b"), "#include not allowed in snippets (provided by the template)"),
        (re.compile(r"\busing\s+namespace\s+std\b"), "'using namespace std' not allowed (provided by the template)"),
        (re.compile(r"\bmain\s*\("), "'main(' not allowed (demos go in tests/demo/)"),
    ]
    for lineno, ln in enumerate(code.split("\n"), 1):
        for rx, msg in rules:
            if rx.search(ln):
                errors.append(f"{rel}:{lineno}: {msg}")
    return errors


def latin1_diagnostics(rel: str, text: str) -> list[str]:
    return [
        f"{rel}:{ln}: character outside Latin-1 {_cp(cp)} (column {col})"
        for ln, col, cp in non_latin1(text)
    ]


# --------------------------------------------------------------------------
# Discovery + validation
# --------------------------------------------------------------------------
def discover_snippets(root: Path) -> list[str]:
    """All src/**/*.cpp as posix paths relative to root (sorted)."""
    src = root / "src"
    if not src.is_dir():
        return []
    return sorted(p.relative_to(root).as_posix() for p in src.rglob("*.cpp"))


def _read_snippet(root: Path, rel: str, report: Report) -> str | None:
    try:
        return (root / rel).read_text(encoding="utf-8")
    except UnicodeDecodeError:
        report.errors.append(f"{rel}:1: the file is not valid UTF-8")
        return None


def validate(root: Path, sections: list[Section]) -> Report:
    report = Report()
    seen: dict[str, int] = {}
    listed: set[str] = set()

    for sec in sections:
        for e in sec.items:
            listed.add(e.path)
            if e.path in seen:
                report.errors.append(
                    f"{MANIFEST_NAME}:{e.line}: duplicate entry '{e.path}' (already listed on line {seen[e.path]})"
                )
                continue
            seen[e.path] = e.line
            if not (root / e.path).is_file():
                kind = "tex file" if e.kind == "tex" else "file"
                report.errors.append(f"{MANIFEST_NAME}:{e.line}: {kind} not found: {e.path}")
                continue
            if e.kind != "code" or not e.path.startswith("src/"):
                continue
            text = _read_snippet(root, e.path, report)
            if text is None:
                continue
            meta = parse_header(text)
            herr, hwarn = header_diagnostics(e.path, meta)
            report.errors += herr
            report.warnings += hwarn
            if meta.fields.get("Status") == "broken":
                report.errors.append(
                    f"{e.path}:{meta.lines['Status']}: Status: broken but listed in {MANIFEST_NAME}:{e.line}"
                )
            report.errors += lint_snippet(e.path, text)
            report.errors += latin1_diagnostics(e.path, text)

    for rel in discover_snippets(root):
        if not SNIPPET_PATH_RE.match(rel):
            report.errors.append(f"{rel}:1: invalid name (each segment must match [a-z0-9_])")
            continue
        if rel in listed:
            continue
        text = _read_snippet(root, rel, report)
        if text is None:
            continue
        meta = parse_header(text)
        if meta.fields.get("Status") == "broken":
            report.infos.append(f"INFO quarantine: {rel} (Status: broken, not in manifest)")
        else:
            report.errors.append(f"{rel}:1: orphan: not listed in {MANIFEST_NAME}")
    return report


# --------------------------------------------------------------------------
# Compile gate
# --------------------------------------------------------------------------
def _abs_posix(root: Path, rel: str) -> str:
    # Windows-style absolute path with forward slashes ("D:/...") works with MinGW
    # and Linux alike (MSYS "/d/..." paths do not).
    return (root / rel).resolve().as_posix()


def _relativize(root: Path, output: str) -> str:
    base = root.resolve().as_posix() + "/"
    out = output.replace(base, "")
    out = out.replace(base.replace("/", "\\"), "")
    return out


def compile_tu(gxx: str, root: Path, includes: list[str]) -> tuple[bool, str]:
    """Compile a TU made of #include of absolute paths, provided via stdin."""
    tu = "".join(f'#include "{_abs_posix(root, rel)}"\n' for rel in includes)
    proc = subprocess.run(
        [gxx, *GXX_FLAGS, "-x", "c++", "-"],
        input=tu,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        cwd=str(root),
    )
    out = _relativize(root, (proc.stdout or "") + (proc.stderr or "")).strip()
    return proc.returncode == 0, out


def compile_one(gxx: str, root: Path, rel: str) -> tuple[bool, str]:
    """TU = template + snippet."""
    return compile_tu(gxx, root, [TEMPLATE_REL, rel])


def check(root: Path = ROOT, jobs: int | None = None) -> int:
    manifest = root / MANIFEST_NAME
    if not manifest.is_file():
        print(f"{MANIFEST_NAME}: not found in {root.as_posix()}")
        return 1

    sections, perrors = parse_manifest(manifest)
    report = validate(root, sections)
    report.errors = perrors + report.errors

    for w in report.warnings:
        print(w)
    for i in report.infos:
        print(i)
    for e in report.errors:
        print(f"ERROR {e}")

    gxx = shutil.which("g++")
    if gxx is None:
        print("g++ not found in PATH (install MinGW g++ or build-essential)")
        # Lint errors take precedence: exit 1 means "fix your sources".
        return 1 if report.errors else 2

    # Template first; abort if it fails.
    tpl = root / TEMPLATE_REL
    if not tpl.is_file():
        print(f"ERROR {TEMPLATE_REL}: template not found")
        return 1
    ok, out = compile_tu(gxx, root, [TEMPLATE_REL])
    if not ok:
        print(f"ERROR {TEMPLATE_REL}: the template does not compile; aborting without compiling snippets")
        print(out)
        return 1
    if out:
        print(out)

    targets: list[str] = []
    for sec in sections:
        for e in sec.items:
            if (
                e.kind == "code"
                and e.path.startswith("src/")
                and e.path not in targets
                and (root / e.path).is_file()
            ):
                targets.append(e.path)

    n_ok = n_fail = 0
    workers = jobs if jobs and jobs > 0 else (os.cpu_count() or 2)
    with ThreadPoolExecutor(max_workers=workers) as pool:
        results = list(pool.map(lambda r: (r, *compile_one(gxx, root, r)), targets))
    for rel, good, out in results:
        if good:
            n_ok += 1
            if out:
                print(f"WARNING {rel}:\n{out}")
        else:
            n_fail += 1
            print(f"ERROR compilation failed: {rel}\n{out}")

    print(f"Compilation summary: {n_ok} ok / {n_fail} failed")
    if report.errors or n_fail:
        print(f"check: FAIL ({len(report.errors)} validation errors, {n_fail} failed compilations)")
        return 1
    print("check: OK")
    return 0


# --------------------------------------------------------------------------
# LaTeX generation
# --------------------------------------------------------------------------
BUILD_DIR = "build"
MONTHS = (
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December",
)
PDF_NAMES = {
    "read": "testigos-de-turing-teambook.pdf",
    "print": "testigos-de-turing-teambook-print.pdf",
}
_TEX_ESCAPES = {
    "\\": r"\textbackslash{}",
    "{": r"\{",
    "}": r"\}",
    "$": r"\$",
    "&": r"\&",
    "#": r"\#",
    "_": r"\_",
    "%": r"\%",
    "^": r"\textasciicircum{}",
    "~": r"\textasciitilde{}",
}


def texify(s: str) -> str:
    """Escape LaTeX specials in a single pass (no double escaping)."""
    return "".join(_TEX_ESCAPES.get(ch, ch) for ch in s)


def render_contents(sections: list[Section]) -> str:
    """One multicols block per section, followed by clearpage."""
    out: list[str] = ["% Generated by scripts/notebook.py tex; do not edit."]
    for sec in sections:
        out.append(rf"\section{{{texify(sec.title)}}}")
        out.append(r"\begin{multicols*}{2}")
        for e in sec.items:
            if e.kind == "tex":
                if e.title:
                    out.append(rf"\subsection{{{texify(e.title)}}}")
                out.append(rf"\input{{{e.path}}}")
            else:
                t = texify(e.title)
                out.append(rf"\subsection{{{t}}}")
                out.append(rf"\NBpath{{{texify(e.path)}}}")
                out.append(rf"\lstinputlisting[style=cpp]{{{e.path}}}")
        out.append(r"\end{multicols*}")
        out.append(r"\clearpage")
        out.append("")
    return "\n".join(out)


def render_meta(today=None) -> str:
    import datetime

    d = today or datetime.date.today()
    date = f"{MONTHS[d.month - 1]} {d.day}, {d.year}"
    return "\n".join(
        [
            "% Generated by scripts/notebook.py tex; do not edit.",
            rf"\newcommand{{\NBdate}}{{{date}}}",
            r"\hypersetup{pdftitle={Team Notebook for Regional 2026 - Testigos De Turing},"
            r"pdfauthor={Testigos De Turing},"
            r"pdfsubject={Competitive programming team notebook}}",
            "",
        ]
    )


def _write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8", newline="\n")


def tex(root: Path = ROOT) -> int:
    manifest = root / MANIFEST_NAME
    if not manifest.is_file():
        print(f"{MANIFEST_NAME}: not found in {root.as_posix()}")
        return 1
    sections, errors = parse_manifest(manifest)
    if errors:
        for e in errors:
            print(f"ERROR {e}")
        return 1
    _write(root / BUILD_DIR / "contents.tex", render_contents(sections))
    _write(root / BUILD_DIR / "meta.tex", render_meta())
    print(f"tex: generated {BUILD_DIR}/contents.tex and {BUILD_DIR}/meta.tex")
    return 0


def pretex_arg(platform: str | None = None) -> str:
    r"""-usepretex argument that defines \PRINTMODE.

    On Windows, latexmk (perl) strips backslashes and braces unless the value
    is wrapped in literal quotes; on POSIX the literal quotes would be wrong.
    """
    value = r"\def\PRINTMODE{1}"
    if (platform or sys.platform) == "win32":
        return f'-usepretex="{value}"'
    return f"-usepretex={value}"


def latexmk_cmd(latexmk: str, theme: str) -> list[str]:
    cmd = [
        latexmk, "-pdf", "-interaction=nonstopmode", "-halt-on-error", "-file-line-error",
        f"-outdir={BUILD_DIR}/{theme}", f"-jobname=notebook-{theme}",
    ]
    if theme == "print":
        cmd.append(pretex_arg())
    cmd.append("tex/notebook.tex")
    return cmd


def _log_excerpt(log: Path, limit: int = 40) -> str:
    if not log.is_file():
        return "(no log)"
    lines = log.read_text(encoding="utf-8", errors="replace").splitlines()
    err = re.compile(r"^(!|[^\s:]+:\d+: )")
    hits = [i for i, l in enumerate(lines) if err.match(l)]
    if not hits:
        return "\n".join(lines[-limit:])
    start = hits[0]
    return "\n".join(lines[start : start + limit])


def run_latexmk(root: Path, latexmk: str, theme: str) -> int:
    print(f"pdf[{theme}]: latexmk ...")
    proc = subprocess.run(
        latexmk_cmd(latexmk, theme), cwd=root, stdin=subprocess.DEVNULL,
        capture_output=True, text=True, encoding="utf-8", errors="replace",
    )
    outdir = root / BUILD_DIR / theme
    built = outdir / f"notebook-{theme}.pdf"
    if proc.returncode != 0 or not built.is_file():
        print(f"ERROR latexmk failed (theme {theme}, exit {proc.returncode}); relevant log:")
        print(_log_excerpt(outdir / f"notebook-{theme}.log"))
        return 1
    dest = root / BUILD_DIR / PDF_NAMES[theme]
    shutil.copyfile(built, dest)
    print(f"pdf[{theme}]: {dest.relative_to(root).as_posix()}")
    return 0


def pdf(root: Path = ROOT, theme: str = "both") -> int:
    latexmk = shutil.which("latexmk")
    if latexmk is None:
        print("latexmk not found in PATH (install TeX Live/MiKTeX with latexmk)")
        return 2
    code = tex(root)
    if code:
        return code
    themes = ["read", "print"] if theme == "both" else [theme]
    for t in themes:
        if run_latexmk(root, latexmk, t):
            return 1
    return 0


# --------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------
def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog="notebook.py", description="Testigos de Turing notebook")
    sub = p.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("check", help="validate the manifest and compile snippets")
    c.add_argument("--jobs", type=int, default=None, help="parallel compilations")
    sub.add_parser("tex", help="generate build/contents.tex and build/meta.tex")
    d = sub.add_parser("pdf", help="generate the PDFs with latexmk")
    d.add_argument("--theme", choices=["read", "print", "both"], default="both")
    return p


def main(argv: list[str] | None = None) -> int:
    for stream in (sys.stdout, sys.stderr):
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except Exception:
            pass
    try:
        args = build_parser().parse_args(argv)
    except SystemExit as e:
        return 2 if e.code not in (0, None) else 0
    if args.cmd == "check":
        return check(ROOT, args.jobs)
    if args.cmd == "tex":
        return tex(ROOT)
    if args.cmd == "pdf":
        return pdf(ROOT, args.theme)
    return 2


if __name__ == "__main__":
    sys.exit(main())
