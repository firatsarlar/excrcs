# c1.cpp — `constexpr` vs `const` for `kBanner`

The header banner is one compile-time constant:

```cpp
constexpr auto kBanner =
    "-----------------------------------------\n"
    "RAW FILE LIST GENERATED FOR LLM PROCESSING:\n"
    "-----------------------------------------\n";
std::cout << kBanner;
```

`auto` deduces `const char*`; the three adjacent literals concatenate at compile time. The qualifier applies to the pointer — the char data sits in static read-only storage either way.

⸻

## Difference

- `const` — read-only after init; initialization *may* occur at runtime.
- `constexpr` — value fixed at compile time (implies `const`); initializer *must* be a constant expression, checked by the compiler.

⸻

## Why constexpr here

- Guard rail — if the banner ever depends on a runtime value, it fails to compile instead of silently doing runtime work.
- Constant-expression use — feeds `static_assert`, array bounds, template args; a runtime `const` cannot.
- Intent — signals "fixed build-time constant," stronger than `const`'s "won't be reassigned."

⸻

## Honest caveat

For `std::cout << kBanner;`, `const char* const` and `constexpr const char*` emit identical code — same binary, same perf. `constexpr` is the modern idiom for compile-time-known data and future-proofs the line; it is not a speed win here.

⸻

## Extensions list — `array<string_view>`, not `vector<string>`

```cpp
static constexpr std::array<std::string_view, 6> kImageExts = {
    ".jpg", ".jpeg", ".png", ".webp", ".avif", ".svg"
};
```

C++17 can't make `std::vector<std::string>` `constexpr` — neither is a literal type (both heap-allocate). The literal-type equivalent is `std::array<std::string_view, N>`: fixed size, each `string_view` points at a static string literal — zero allocation, table baked into the binary. The lookup is unchanged: `std::find(kImageExts.begin(), kImageExts.end(), ext)` still works because `string_view == std::string` compares by value.

