# C++ standard modernization — c1.cpp

Baseline: `c1.cpp` targets **C++17** (Apple clang default). `c1_v20.cpp` is the same
program rewritten in **C++20** — verified to compile (`-std=c++20 -Wall -Wextra`)
and produce identical output on this toolchain.

⸻

## C++20 (realistic today) — see `c1_v20.cpp`

- **Ranges** — `views::filter | views::transform` replace the manual loop +
  `if continue` + `push_back`.
- **`std::ranges::sort` / `find` / `transform`** — range-based algorithms.
- **`std::format`** — typed formatting, replaces `<<` chains.
- **CTAD** on `std::array` — `constexpr std::array kImageExts = {".jpg"sv, ...}`.

Caveat: the `recursive_directory_iterator` is bound to a named lvalue before the
pipe — a temporary rvalue isn't a `viewable_range` in C++20. Exceptions during
traversal still need the try/catch; `skip_permission_denied` only covers the
common case.

⸻

## C++23 (mostly on libc++; aspirational pieces noted)

- **`std::print` / `std::println`** — replace iostream (libc++-version dependent).
- **`std::ranges::to<std::vector>()`** — materialize the view in one step.
- **`std::ranges::contains`** — vs `find(...) != end()`.
- **`std::expected<T, std::error_code>`** — error path without exceptions.
- **`import std;`** — one line for all includes; needs module build support Apple
  clang doesn't ship cleanly yet.

⸻

## C++26 ("2026") — little applies here

A ~60-line file lister doesn't benefit from the headline features:

- **Reflection**, **senders / `std::execution`**, **`#embed`** — not applicable /
  over-engineering.
- **Contracts** (`pre`/`post`) — the only mild fit; not implemented in mainstream
  compilers yet.

⸻

## Build

- C++17: `g++ -std=c++17 c1.cpp -o asset_scraper`
- C++20: `g++ -std=c++20 c1_v20.cpp -o asset_scraper`

Related: `asset-scraper.md`, `c1-const-vs-constexpr.md`
