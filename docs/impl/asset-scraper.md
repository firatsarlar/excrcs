# asset_scraper — design & decisions

`d/c1.cpp` — a C++17 CLI that recursively walks a directory, keeps image files,
and prints their sorted relative paths for portfolio ingestion. Practice project;
the priority is minimal, idiomatic modern C++.

⸻

## Pipeline

walk (`recursive_directory_iterator`) → keep regular files → filter by extension
(case-insensitive) → collect relative paths → sort → print. One `try/catch` at the
filesystem boundary; no per-entry error handling.

⸻

## Decisions

- **constexpr constants** for banner / divider / error text; image extensions are
  `constexpr std::array<std::string_view, 6>` — a compile-time table, zero heap
  allocation. Rationale in `c1-const-vs-constexpr.md`.
- **Hardened walk** — `recursive_directory_iterator(path, skip_permission_denied)`
  so one locked subdirectory doesn't abort the whole scan.
- **Cached status** — `entry.is_regular_file()` (member; uses the iterator's cached
  dirent type) instead of `fs::is_regular_file(entry.status())`, which re-stats
  every entry.
- **The `is_regular_file` gate** skips the subdirectory entries the recursive
  iterator emits, and guards against a directory named like an image (`cover.jpg`).
- **Helpers kept**: `to_lower`, `relative_path(const fs::path&, const fs::path&)`
  (the `lexically_relative(...).generic_string()` combo is non-obvious — naming it
  earns its place). **Dropped**: `exists_and_is_directory` — `fs::is_directory`
  already returns false for a missing path, so the `exists() &&` was redundant.

⸻

## Considered, not done

- **Make it a class / global `path`** — not needed for the code: it's a ~65-line
  run-once pipeline; a class is ceremony and a global is hidden state. The pure
  `std::vector<std::string> scan_images(root)` free function with a thin `main` is
  the step *before* a class. A class earns its keep only once there's CLI config,
  library reuse, or multi-scan state. **Done anyway as a C++ learning exercise** —
  see `c1-class-refactor.md` (`AssetScraper`: scan-once, shared `results_`,
  iterators encapsulated).
- **Allocation-free extension match** — `to_lower(...).string()` allocates a string
  per file, but `.extension().string()` allocates anyway and the loop is I/O-bound,
  so an in-place case-insensitive compare adds complexity for no measurable gain.
  Left as-is. (Would make a good learning exercise.)
- **Hash set for extensions** — a `std::unordered_set` lookup is *slower* than a
  linear scan over a 6-element contiguous `string_view` array (hashing + allocation
  + pointer-chasing beat by cache locality). Not done.

⸻

## Repo / build

- Build + run: `cd d && ./build.sh`. Tests: `./test.sh`. Debug: VS Code F5 (lldb-dap).
- Published: https://github.com/firatsarlar/excrcs (public).
- The compiled `asset_scraper` binary is gitignored.

⸻

Related: `c1-const-vs-constexpr.md`, `vscode-debug-setup.md`, `asset-scraper-tests.md`
