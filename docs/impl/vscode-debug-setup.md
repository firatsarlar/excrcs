# VS Code — clangd-only IntelliSense + Debugging

False red squiggles under every C++ `#include`. Root cause — two language services ran at once: clangd (correct, 0 errors) and Microsoft cpptools IntelliSense (no configured include paths → underlined every `#include <...>`). Fix — clangd as sole authority, cpptools IntelliSense off, lldb-dap debugging added.

⸻

## Diagnosis

- `clang++ -std=c++17 -Wall c1.cpp` — compiles clean, zero warnings.
- `clangd --check=c1.cpp` — builds AST with 0 real diagnostics. The "2 errors" were clangd's internal `ExtractFunction` tweak self-tests, not code.
- Conclusion — squiggles came from cpptools IntelliSense, not clangd and not the source.

⸻

## What changed

- `.vscode/settings.json` — `C_Cpp.intelliSenseEngine: disabled`; clangd args (`--background-index`, `--clang-tidy`); lldb-dap executable path.
- `.vscode/tasks.json` — default build task `mkdir -p dist && g++ -std=c++20 -g -O0 c1.cpp -o dist/asset_scraper` (cwd `d/`).
- `.vscode/launch.json` — `lldb-dap` launch, cwd `d/`, `preLaunchTask` = debug build.
- `.gitignore` — selective `.vscode/*` pattern keeping shared config tracked, user-local ignored.

⸻

## Verify

1. Reload window (Cmd+Shift+P → Developer: Reload Window) — applies the engine change; squiggles gone.
2. F5 — builds with `-g`, launches under lldb-dap; breakpoints, stepping, variable inspection work.
3. Program lists 8 image fixtures from `d/images` — confirms run cwd is `d/`.

⸻

## Known limitations

- `lldb-dap.executable-path` hardcodes Xcode's path — machine-specific; drop the line for portability (extension auto-detects).
- `ngsoftware.mql-clangd` is a second clangd (scoped to `.mq4/.mq5`) — disable per-workspace if doubled diagnostics ever appear.
