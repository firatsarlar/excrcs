# Plan — c1.cpp → AssetScraper class (C++17)

## Problem statement

Convert the run-once scraper in `d/c1.cpp` from an everything-in-`main` pipeline
into an `AssetScraper` class. Driven as a **C++ learning exercise**, not a need —
the design doc previously ruled a class out as ceremony for a 65-line tool.

## Pain points / current state

- `main` does walk + filter + sort + stdout + file write inline; no reuse, hard
  to test a single stage.
- Raw `begin()/end()` iterator pairs at the call site (`std::find`, `std::sort`).
- The results vector is a local; consumers (print, file) are wired ad hoc.

## Proposed solution

Encapsulate the pipeline. `results_` is computed **once** by `scan()` and
**shared** (by `const&`) with every consumer — no recompute, no copy.

Iterators are **encapsulated, not removed** (C++17 has no `std::ranges`):
`std::find`/`std::sort` live inside the methods; `main` never touches iterators.

### Interface (signatures only)

```cpp
class AssetScraper {
public:
    explicit AssetScraper(fs::path root);

    std::size_t scan();                                   // walk+filter+sort → results_, returns count
    const std::vector<std::string>& paths() const noexcept;   // shared, no copy

    void print(std::ostream&) const;                      // human-formatted "- path"
    bool write(const fs::path& out) const;                // clean path-per-line; false on open fail

private:
    static bool is_image(const fs::path&);                // case-insensitive ext match
    static std::string relative_to(const fs::path&, const fs::path& base);

    fs::path root_;
    std::vector<std::string> results_;
};
```

### Usage (thin main)

```cpp
AssetScraper scraper(".");
scraper.scan();
scraper.print(std::cout);                 // reads results_
const bool ok = scraper.write("images.txt");   // reads the SAME results_
```

### Decisions / trade-offs

- `print()` and `write()` are **two passes** over `results_`. For this list size
  it is negligible and I/O-bound; the win is single-responsibility methods reading
  one shared vector. (Merging into one pass would need an awkward combined method.)
- `to_lower` stays file-local; `relative_to` becomes a private static (only the
  class needs it).
- `kImageExts` becomes a `static constexpr` class member (unchanged data).
- Still C++17, single file, zero dependencies.

## Files to modify

- `d/c1.cpp` — the conversion.
- `docs/impl/asset-scraper.md` — update the "Considered, not done → class" note to
  point at this exercise.
- `docs/impl/c1-class-refactor.md` — implementation notes (created after coding).

## Verification steps

1. `cd d && g++ -std=c++17 -Wall -Wextra c1.cpp -o asset_scraper` — no warnings.
2. `./asset_scraper` — same stdout banner; `images.txt` has the same 8 sorted paths.
3. `./test.sh` — existing suite still passes.
