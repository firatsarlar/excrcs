# Impl — c1.cpp → AssetScraper class (C++17)

Learning-exercise refactor of `d/c1.cpp` from an everything-in-`main` pipeline to
an `AssetScraper` class. Plan: `docs/plans/c1-class-refactor.md`.

## What changed

- `d/c1.cpp` — introduced `class AssetScraper`:
  - `scan()` walks the root, filters image files, sorts — populates `results_`
    **once** and returns the count.
  - `paths()` exposes that single vector by `const&` (no copy); `print(os)` and
    `write(out)` both read it — **results are shared, computed once.**
  - `is_image()` and `relative_to()` are private statics; `kImageExts` is a
    `static constexpr` class member. `to_lower` stays a file-local free function.
  - `main` is thin: construct → `scan()` (in the try/catch) → `print` → `write`.

## Design notes

- **Iterators encapsulated, not removed.** C++17 has no `std::ranges`, so
  `std::find`/`std::sort` still live *inside* the methods — but no `begin()/end()`
  pair appears outside the class. (Full removal would be C++20; see `c1_v20.cpp`.)
- **Two passes over `results_`** (`print` then `write`). Both read the same shared
  vector — no recompute. For this list size it is negligible and I/O-bound; the
  win is single-responsibility methods. Single-pass would need a combined
  `emit(stream, file)` method (uglier API), deliberately not taken.

## How to verify

1. `cd d && g++ -std=c++17 -Wall -Wextra c1.cpp -o asset_scraper` — no warnings.
2. `./test.sh` — 12/12 pass (stdout `- path` listing unchanged).
3. `./asset_scraper` — same banner; `images.txt` = 8 sorted clean paths.

## Known limitations / behavior changes

- **Exit codes tightened**: not-a-directory and filesystem errors now `return 1`
  (the pre-refactor code printed the error but still returned 0). Untested by
  `test.sh`; more correct. The happy path is unchanged.
- Root is still hardcoded to `"."`. A class now makes constructor-injected root /
  configurable extensions a natural next step if ever needed.
