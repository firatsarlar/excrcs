# Scraper design review — anti-patterns and principles

A review of the c1 `AssetScraper` class that shaped the c2 (`Images`) rewrite.
Kept as a design reference for the playground.

## Anti-patterns the class fell into

1. **Procedural code wearing a class.** `scan()` mutates state, `print`/`write`
   read it — a free function with extra steps. Worse, *temporal coupling*:
   `AssetScraper s("."); s.print(cout);` compiles and silently prints nothing.
   The type permits an invalid state. Fix: make illegal states unrepresentable —
   scan eagerly (c2's `Images::from(root)`), or just return the vector.
2. **I/O welded into the domain type.** A scraper should scrape; baking
   `ostream`/file I/O in makes it untestable and dual-responsibility. Data out,
   I/O at the edges. (c2 keeps the sinks but exposes `items()` so the data stays
   reachable — a conscious tradeoff for the fluent API.)
3. **Leaky, redundant API.** `paths()` returns `const vector<string>&`, pinning
   the internal container into the public interface; `scan()` returns a count
   already available from `.size()`.
4. **Verbose where it should be silent.** Error-string constants, banner,
   narrating comments (`// reads results_`). Comments are for complex logic only.
5. **Feature-demonstrating.** `constexpr` / `string_view` / static members
   deployed to look modern rather than as the simplest thing that works.

## Principles (applied in c2)

- Simplest thing first: a free function returning a value beats a premature class.
- No invalid states: construct fully-formed (eager factory).
- Keep data reachable even behind a fluent facade (`items()`).
- Minimal surface, short lines, comments only when they earn it.

## Exceptions / no try-catch

Some shops ban exceptions (Google style guide; LLVM, games, embedded, HFT compile
`-fno-exceptions`): deterministic control flow, smaller binaries, ABI/embedded
constraints, exception-unsafe legacy. Alternatives: error codes,
`std::optional` / `std::expected` (C++23), status types (`absl::Status`),
`assert`/`abort` for programmer bugs.

For `std::filesystem` specifically: **every call has a non-throwing overload
taking `std::error_code&`.** The scrapers use this for `create_directories(...)`.

Tradeoff that bites here: a *fully* no-throw directory walk needs the manual
`it.increment(ec)` loop — which reintroduces explicit iterators. The range-`for`
hides the iterator but its `operator++` throws on error. So **no-exceptions <-> 
no-iterators is a genuine conflict** in the walk. c1 wraps the walk in try/catch;
c2 relies on `skip_permission_denied` + an `is_directory` precheck (no try/catch,
keeps the range-`for`), leaving only a rare mid-walk error unhandled. The fully
exception-free variant is sketched in `docs/plans/c3-exception-free-scraper.md`.
