# Impl — c2.cpp: fluent image scraper (C++20)

A jQuery-style take on the scraper, built in the light of the c1 review.
Plan: `docs/plans/c2-fluent-scraper.md`.

## What it is

```cpp
Images::from(".").sort().print().save("dist/images.txt");
```

`d/c2.cpp` — a fluent `Images` value type. `from(root)` scans eagerly; every
method returns `Images&` so calls chain in any order; `.each(fn)` is the
jQuery-style sink. Builds with `-std=c++20 -Wall -Wextra`, no warnings.

## How the review fed the design

- **No invalid state** — eager `from()` factory; there is no "forgot to scan()".
- **Data reachable** — `items()` exposes the vector, so the type is testable /
  reusable despite carrying the I/O sinks (the one review tradeoff, taken on
  purpose for the chaining).
- **No explicit iterators** — `std::ranges::sort`, `std::ranges::any_of` (ext
  match), `std::ranges::for_each` (in-place lowercase), range-`for` everywhere.
  Only `recursive_directory_iterator` appears, named in a range-`for` —
  unavoidable for traversal.
- **Quiet** — no banner, no narrating comments, `main` is the one-liner + an
  `ok()` check.

## Decisions

- **Total-freedom chaining**: all methods return `Images&` (non-const). `print`
  and `save` therefore aren't const — a deliberate white lie for fluency.
- **`save()` is silent**: never throws; sets `ok_`. `main` returns non-zero if a
  write failed (`.ok()`).
- **`from()` returns by value**; the one-liner copies the temporary into `imgs`
  (~8 strings — negligible).

## How to verify

1. `cd d && mkdir -p dist && g++ -std=c++20 -Wall -Wextra c2.cpp -o dist/c2` — no warnings.
2. `./dist/c2` — 8 sorted paths; `dist/images.txt` matches c1's set.

## Known limitations

- Mid-iteration filesystem errors (e.g. a dir deleted during the walk) propagate
  out of `collect()` and terminate — only the `is_directory` precheck is guarded.
  c1 wraps the walk in try/catch; c2 trades that for minimalism. Add a boundary
  catch if it ever runs on volatile trees.
- Root is hardcoded to `"."`, like c1.
