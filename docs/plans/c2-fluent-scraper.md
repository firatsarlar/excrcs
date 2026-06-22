# Plan — c2.cpp: fluent image scraper (C++20)

## Goal

A fresh take on the scraper that fixes the c1 review *and* reads like jQuery:
chainable, beautiful, short lines, no explicit iterators.

## Design (what & why)

- **`Images` value type, eager factory.** `Images::from(root)` scans on
  construction -> no "forgot to scan()" invalid state (c1's temporal-coupling sin).
- **Fluent.** Every method returns `Images&` — any order, jQuery-style:
  ```cpp
  Images::from(".").sort().print().save("images.txt");
  ```
- **Data stays reachable.** `items()` exposes the vector -> testable / reusable
  even though I/O sinks live on the type. That is the one review tradeoff, taken
  on purpose for the chaining requested.
- **No iterators.** `std::ranges::sort`, `std::ranges::any_of` for the extension
  match, range-`for` for walk/print. Only `recursive_directory_iterator` appears
  (named in a range-`for`) — unavoidable for traversal.
- **Quiet.** No banner, no narrating comments. `main` is one line.

## Interface (signatures only)

```cpp
class Images {
public:
  static Images from(fs::path root);              // scans eagerly

  Images& sort();                                 // std::ranges::sort
  template <class F> Images& each(F&& fn);        // jQuery .each
  Images& print(std::ostream& = std::cout);
  Images& save(const fs::path& file);             // silent; sets ok()

  const std::vector<std::string>& items() const noexcept;
  std::size_t size() const noexcept;
  bool ok() const noexcept;                        // false if last save() failed
};
```

## Decisions / tradeoffs

- **C++20** (ranges -> no iterators). Repo already builds C++20.
- **Total-freedom chaining**: every method returns `Images&` (non-const). Any
  order. Cost: `print`/`save` aren't truly const — a small white lie for fluency.
- **save() is silent**: never throws; sets `ok_`, checked via `.ok()` after the
  chain. `main` returns non-zero if it failed.
- Drops c1's banner/divider. Intentional behavior diff for minimalism.

## Files

- `d/c2.cpp` — new. c1.cpp untouched.

## Verify

1. `g++ -std=c++20 -Wall -Wextra c2.cpp -o c2` — no warnings.
2. Run in `d/images`: same 8 sorted paths; `images.txt` matches c1's output.
