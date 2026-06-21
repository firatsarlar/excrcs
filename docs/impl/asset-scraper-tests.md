# Asset Scraper — Automated Tests

## What was changed
- `d/c1.cpp` — fixed 3 defects:
  - case-insensitive extension match (`to_lower` on extension) → catches `.JPG`, `.PNG`, etc.
  - emit relative path via `lexically_relative(path).generic_string()` → same-named files in different dirs stay distinct; `/` separators on all platforms.
  - `std::sort` before printing → deterministic, reproducible ("conflict-free") output.
- `d/images/` — persisted, real 1x1 image fixtures on disk (jpg/jpeg/png/webp + uppercase `BANNER.JPG` + duplicate-name pair under `albums/2024` and `albums/2025` + nested `nested/macro.jpg` + `notes.txt` control).
- `d/test.sh` — build + run + 12 simple asserts (`assert_contains` / `assert_absent` / `assert_eq`); exits non-zero on failure.

## How to verify
```
bash d/test.sh        # expect: 12 passed, 0 failed; exit 0
```

## Known limitations
- `assert_eq "$TOTAL" "8"` is tied to the current fixture set; adding/removing files in `d/images/` requires updating that count.
- Scraper still scans only `.` (cwd); the test `cd`s into `images/` to target it. A CLI path argument was out of scope.
- IDE/clangd flags `fs` as undeclared — false positive from missing `-std=c++17` / `compile_commands.json`; `g++` build is clean.
