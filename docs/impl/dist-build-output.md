# Impl — dist/ build-output folder

All generated artifacts (compiled binaries + the generated `images.txt`) now live
under `dist/`, ignored by a single `.gitignore` rule.

## What changed

- **`.gitignore`** — replaced the per-file entries (`asset_scraper`, `images.txt`)
  with one `dist/` rule, plus a stray-compile backstop (`a.out`, `*.o`, ...).
  `dist/` matches the folder wherever it appears.
- **`d/c1.cpp` / `d/c2.cpp`** — output path is now `dist/images.txt`; both create
  the parent dir first via `fs::create_directories(..., ec)` — the **non-throwing**
  `error_code` overload, so no exceptions are introduced.
- **`d/build.sh`, `d/test.sh`** — `mkdir -p dist`, binaries built to
  `dist/asset_scraper`.
- **`.vscode/tasks.json` / `launch.json`** — debug build + launch target moved to
  `dist/asset_scraper` (task now `mkdir -p dist && g++ ...`).
- **`CLAUDE.md`, `docs/impl/vscode-debug-setup.md`** — build commands + layout
  updated; added `c2.cpp` and `dist/` to the layout.

## How to verify

1. `cd d && ./build.sh` — produces `dist/asset_scraper` and `dist/images.txt`.
2. `./test.sh` — 12/12.
3. `git status` — no stray untracked binaries; `git check-ignore d/dist/...` passes.

## Notes

- The tools write `dist/images.txt` **relative to cwd**. Run from `d/` (the
  canonical build) -> `d/dist/`. `test.sh` runs the binary from `d/images/`, so it
  creates `d/images/dist/` — harmless and ignored (the `dist/` rule matches it),
  and it holds only a `.txt`, so it never affects the image walk.
- `error_code` is ignored on `create_directories`: if the dir can't be made, the
  subsequent open simply fails (c1 returns false / c2 sets `ok()` false).
