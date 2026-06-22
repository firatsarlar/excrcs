# CLAUDE.md

## What this is

A personal practice repo for **poking around C++** — exploring the C++17 standard
library (filesystem, string_view, constexpr) by building small command-line tools.
This is learning/exploration, not production software: priority is minimal,
idiomatic, modern C++ with zero third-party dependencies.

It also backs a GitHub Pages portfolio (single-file `index.html`); the C++ side
feeds it by scraping local asset directories into a file list. See `d/ctx.md`.

Published: https://github.com/firatsarlar/excrcs

## Layout

- `d/` — the C++ playground.
  - `c1.cpp` — `asset_scraper`: recursively lists image files under a directory,
    sorted, for portfolio ingestion. (Canonical build, C++17.)
  - `c1_v20.cpp` — the same program in C++20 (ranges + `std::format`), kept for
    comparison; see `docs/impl/cpp-modernization.md`.
  - `c2.cpp` — `Images`: a fluent/chainable C++20 rewrite
    (`Images::from(".").sort().print().save(...)`); see `docs/impl/c2-fluent-scraper.md`.
  - `build.sh` — `g++ -std=c++17 c1.cpp -o dist/asset_scraper && ./dist/asset_scraper`
  - `test.sh` — scraper test suite.
  - `.clangd` — clangd config (C++17 flags; designator inlay hints off).
  - `images/` — test fixtures (nested dirs, albums).
  - `dist/` — build output (binaries + generated `images.txt`); gitignored.
- `docs/impl/` — implementation notes and decisions. **Read these for background.**
- `.vscode/` — clangd-only IntelliSense + lldb-dap debugging.

## Build / run / debug

- Build + run: `cd d && ./build.sh`
  (or `g++ -std=c++17 -Wall -Wextra c1.cpp -o dist/asset_scraper`).
- Tests: `cd d && ./test.sh`.
- Debug: VS Code F5 (lldb-dap; builds with `-g` first, runs from `d/`).

## Conventions

- C++17, zero dependencies, single-file tools.
- Prefer minimal idiomatic modern C++: `constexpr` for fixed data,
  `string_view`/`array` over heap containers where it fits, helpers only when
  they earn their name.
- All build output goes to `dist/` (gitignored) — binaries and the generated
  `images.txt`; never commit it.
- clangd is the single language server; the Microsoft C/C++ IntelliSense engine
  is disabled in `.vscode/settings.json`.

## Background docs

- `docs/impl/asset-scraper.md` — scraper design and the key decisions (start here).
- `docs/impl/cpp-modernization.md` — C++17 → 20/23/26 modernization (and `c1_v20.cpp`).
- `docs/impl/asset-scraper-tests.md` — test suite.
- `docs/impl/vscode-debug-setup.md` — clangd-only setup + how debugging is wired.
- `docs/impl/c1-const-vs-constexpr.md` — constexpr/const choices in c1.cpp.
- `d/ctx.md` — the portfolio project context.
