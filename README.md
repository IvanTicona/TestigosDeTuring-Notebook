# Testigos de Turing - ICPC Team Notebook

ICPC team notebook (team "Testigos de Turing", Universidad Privada Boliviana).
The algorithms live as C++ snippets, and a Python script validates them and builds a LaTeX PDF
(landscape letter, 2 columns) in two themes: read (color) and print (black and white).

## Structure

```
contents.txt        manifest: single source of order and titles for the PDF
template/           template.cpp: shared includes, typedefs and macros (no main)
src/<topic>/*.cpp   library snippets (functions/classes only, no includes or main)
tests/demo/<topic>/ demos with main() that include the template + a snippet
tex/notebook.tex    LaTeX document (cover, contents, header, code style)
tex/assets/         logo; tex/theory/*.tex: optional theory (!tex), created on demand
scripts/notebook.py CLI: check | tex | pdf
build/              generated output (ignored by git)
```

## Requirements

- Python 3 (standard library only).
- `g++` with `-std=gnu++17` support (MinGW on Windows).
- `latexmk` and a LaTeX distribution: MiKTeX (Windows) or TeX Live (Linux/macOS).
  Packages: geometry, fontenc, inputenc, lmodern, microtype, xcolor, graphicx, listings
  (listingsutf8), multicol, fancyhdr, lastpage, tocloft, upquote, hyperref.
- Optional: `pdfinfo`, `pdftotext`, `pdftoppm` (poppler) to inspect the PDFs.

## Usage

```
python scripts/notebook.py check [--jobs N]            # validate and compile every snippet
python scripts/notebook.py tex                         # generate build/contents.tex and build/meta.tex
python scripts/notebook.py pdf [--theme read|print|both]   # generate the PDFs (default: both)
```

- `check`: validates the manifest and the headers, and compiles each snippet with the template
  (`g++ -std=gnu++17 -fsyntax-only -Wall -Wextra`). Exit codes: 0 ok, 1 validation or
  compilation errors, 2 a tool is missing (g++). It ends with a line such as
  `Compilation summary: 18 ok / 0 failed` followed by `check: OK` (or `check: FAIL (...)`).
- `pdf`: runs `tex` and then `latexmk`. Output: `build/testigos-de-turing-teambook.pdf`
  (read) and `build/testigos-de-turing-teambook-print.pdf` (print). TeX error: exit 1
  with the relevant log; no `latexmk`: exit 2. `pdf` does not run `check`.

## Adding a snippet

1. Create `src/<topic>/<name>.cpp` with a `snake_case` name (`[a-z0-9_]`).
2. Write library code only: no `#include`, no `using namespace std`, no `main`. Use what
   `template/template.cpp` already defines (`vi`, `pii`, `ll`, `INF`, `MOD`, ...).
3. Put the header (see below) at the top of the file.
4. List it in `contents.txt` under its section: `src/<topic>/<name>.cpp | English title`.
5. (Optional) Create a demo in `tests/demo/<topic>/<name>.cpp`:

   ```cpp
   #include "template/template.cpp"
   #include "src/<topic>/<name>.cpp"
   int main() { /* example */ }
   ```
6. Run `python scripts/notebook.py check` and then `pdf`.

Snippets with no entry in `contents.txt` (and without `Status: broken`) are a `check` error.

## Header format

Block comment at the top of the snippet:

```cpp
/**
 * Usage: UnionFind UF(N); UF.unionSet(i, j); UF.isSameSet(i, j)
 * Complexity: O(alpha(n)) amortized
 * Source: (optional)
 * Tested: (optional)
 * Note: (optional)
 * Status: untested
 */
```

Recognized fields: `Usage`, `Complexity`, `Source`, `Tested`, `Note` and `Status`.
`Status` is required: `ok`, `untested` or `broken`; `Usage` and `Complexity` are
recommended (`check` warns if they are missing). A `broken` snippet is quarantined: it must
not appear in `contents.txt` (if it is listed, `check` fails) and it is reported as INFO
(`INFO quarantine: ...`). The content must be Latin-1 (the PDF uses `listingsutf8`); `check`
reports the offending codepoint.

## `contents.txt` grammar

```
# comment (whole line); ` #` (space + #) also starts an inline comment
[Section title]                        # opens a section (new page in the PDF)
path/to/snippet.cpp | Title            # snippet: PATH | Title (path relative to the repo root)
!tex tex/theory/file.tex | Title       # includes a .tex file with \input
```

- The first section is `[Template]` with `template/template.cpp | Template`.
- Paths: `src/...*.cpp` or `template/...*.cpp` for code and `tex/...*.tex` for `!tex`, using
  characters `[a-z0-9_/]`; no duplicates.
- Errors are reported as `contents.txt:<line>: message`.
- Titles may contain LaTeX special characters (`_ & % # $`); they are escaped automatically.
  Avoid ` #` (space before `#`) in titles: it is interpreted as a comment
  (use `Union_Find#1`).
- Each snippet is printed under its title (subsection) with the path in gray on the right;
  there are no numbered captions or list of algorithms (the cover contents play that role).

## Demos

`tests/demo/<topic>/*.cpp` are complete programs to try a snippet by hand:

```
g++ -std=gnu++17 -I. tests/demo/dp/lcs.cpp -o lcs && ./lcs
```

## Publishing (local)

Since 2026-11-05 the PDF is built and uploaded from a local machine (GitHub Actions is no
longer triggered on push):

```
python -m unittest discover -s scripts -p "test_*.py"   # script unit tests
python scripts/notebook.py check                       # compile gate
python scripts/notebook.py test --jobs 4               # stress tests
python scripts/notebook.py pdf --theme both            # build/ PDFs (read + print)
python scripts/drive_upload.py --file build/testigos-de-turing-teambook.pdf
```

The Drive upload needs `GDRIVE_FILE_ID` and `GDRIVE_SA_JSON` (the key file contents) in the
environment of that shell only; never commit the key. PowerShell:
`$env:GDRIVE_SA_JSON = Get-Content path\to\key.json -Raw`; bash:
`export GDRIVE_SA_JSON="$(cat path/to/key.json)"`.

## CI, Release and Google Drive (manual only)

The workflow `.github/workflows/build.yml` now runs only manually (`workflow_dispatch`): unit
tests, `check`, installs TeX Live, `pdf --theme both`, uploads the PDFs as an artifact, updates
the `latest` Release (`--clobber`) and updates the PDF on Drive. If `check` or `pdf` fails
nothing is published (and the LaTeX logs are uploaded as an artifact).

Stable downloads: Releases -> `latest` -> `testigos-de-turing-teambook.pdf` (color) and
`testigos-de-turing-teambook-print.pdf` (print).

### Secrets (Settings -> Secrets and variables -> Actions)

| Secret | Purpose |
|---|---|
| `GDRIVE_FILE_ID` | id of the existing PDF file on Drive (from its URL). Without it the Drive step is skipped (`Drive skipped: missing secrets`; the workflow stays green and the Release is still published). |
| `GDRIVE_SA_JSON` | Full JSON of the service account key (primary auth). |
| `GDRIVE_OAUTH_CLIENT_ID`, `GDRIVE_OAUTH_CLIENT_SECRET`, `GDRIVE_OAUTH_REFRESH_TOKEN` | OAuth fallback, used only if `GDRIVE_SA_JSON` is not set. |

### Setting up Drive (once)

1. Create a Google Cloud project and enable the **Google Drive API**.
2. Create a **service account** and a JSON key; store its contents in `GDRIVE_SA_JSON`.
3. On Drive, share the PDF with the service account's `client_email` as **Editor**.
4. Copy the fileId from the file URL into `GDRIVE_FILE_ID`.
5. Test locally, without uploading anything:

```
pip install -r scripts/requirements-drive.txt
python scripts/drive_upload.py --check      # exit 0 if canEdit=true
```

The script only updates the content (media-only PATCH), so the fileId, the name
`testigos-de-turing-teambook.pdf` and the shared links do not change. It never prints secrets.

### Common errors

| Message | Cause and fix |
|---|---|
| `403 insufficientFilePermissions` | The file is not shared as Editor with the `client_email`. |
| `403 storageQuotaExceeded` | A service account has no storage quota of its own to own content: use the **OAuth fallback**. |
| `404` | Wrong `GDRIVE_FILE_ID`, or the file is not shared with the service account. |
| `401` | Invalid or expired credentials. |

### OAuth fallback (only if `storageQuotaExceeded` appears)

1. In the same Cloud project, create an OAuth client of type **Desktop**.
2. **Publish** the app (in "Testing" mode the refresh token expires after 7 days).
3. Obtain the refresh token once locally (e.g. with `google-auth-oauthlib`, outside CI)
   with scope `https://www.googleapis.com/auth/drive`.
4. Store `GDRIVE_OAUTH_CLIENT_ID`, `GDRIVE_OAUTH_CLIENT_SECRET` and `GDRIVE_OAUTH_REFRESH_TOKEN`
   as secrets; the OAuth account must be able to edit the file. Verify with `--check`.

## Notes

- Windows: `latexmk` (perl) requires literal quotes in `-usepretex`; the script already handles it.
- If one `latexmk` run fails and the next one behaves oddly, delete `build/<theme>/`.
