# Plan — c3.cpp: fully exception-free scraper (sketch)

A pending option, not yet built. Captures the no-exceptions walk so the idea
survives a restart.

## Goal

A scraper that never throws — for the no-`-fno-exceptions` style some shops use.
A contrast piece against c2's range-`for` walk.

## Approach

Use `std::filesystem`'s `error_code` overloads throughout, including a manual
directory walk so iteration errors surface as codes rather than exceptions:

```cpp
std::error_code ec;
fs::recursive_directory_iterator it(root, skip, ec), end;
for (; !ec && it != end; it.increment(ec)) {
  // filter + collect; check ec, never throw
}
```

## The tradeoff to accept

This **reintroduces explicit iterators** (`it`, `end`, `it.increment(ec)`) — the
exact thing c2 removed. No-exceptions and no-iterators cannot both hold in the
walk. c3 chooses no-exceptions; c2 chooses no-iterators. Keep both as a
side-by-side comparison (like c1 vs c1_v20).

## Decisions still open

- Fluent (`Images`-style) or plain free function?
- Report errors via `std::expected<std::vector<std::string>, std::error_code>`
  (C++23) or an out-param `error_code` + bool?
- C++20 vs C++23 (for `std::expected`).

## Files

- `d/c3.cpp` — new (not yet written).
