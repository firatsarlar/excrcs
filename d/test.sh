#!/usr/bin/env bash
# Automated tests for the C++ asset scraper (c1.cpp).
# Builds the binary, runs it against the on-disk fixtures in ./images,
# then checks the output with simple asserts. Exits non-zero on any failure.

set -uo pipefail

HERE=$(cd "$(dirname "$0")" && pwd)
BIN="$HERE/asset_scraper"
IMAGES="$HERE/images"
PASS=0
FAIL=0

assert_contains() { # <relative-path> <description>
  if printf '%s\n' "$ENTRIES" | grep -qxF -- "- $1"; then
    PASS=$((PASS + 1)); echo "  ok   - $2"
  else
    FAIL=$((FAIL + 1)); echo "  FAIL - $2 (expected entry '- $1')"
  fi
}

assert_absent() { # <relative-path> <description>
  if printf '%s\n' "$ENTRIES" | grep -qxF -- "- $1"; then
    FAIL=$((FAIL + 1)); echo "  FAIL - $2 (did not expect entry '- $1')"
  else
    PASS=$((PASS + 1)); echo "  ok   - $2"
  fi
}

assert_eq() { # <actual> <expected> <description>
  if [ "$1" = "$2" ]; then
    PASS=$((PASS + 1)); echo "  ok   - $3"
  else
    FAIL=$((FAIL + 1)); echo "  FAIL - $3 (got '$1', want '$2')"
  fi
}

echo "== build =="
g++ -std=c++17 "$HERE/c1.cpp" -o "$BIN" || { echo "BUILD FAILED"; exit 1; }
echo "  ok   - compiles with -std=c++17"

echo "== run scraper against ./images =="
OUT=$(cd "$IMAGES" && "$BIN")
ENTRIES=$(printf '%s\n' "$OUT" | grep '^- ' || true)

echo "== asserts =="
# happy path: each supported extension is listed
assert_contains "landscape.jpg"          "lists top-level .jpg"
assert_contains "portrait.jpeg"          "lists .jpeg"
assert_contains "diagram.png"            "lists .png"
assert_contains "art.webp"               "lists .webp"
# recursion + relative paths
assert_contains "nested/macro.jpg"       "recurses into subdirectories (relative path)"
# case-insensitive extension match
assert_contains "BANNER.JPG"             "matches uppercase extension"
# non-image control is rejected
assert_absent   "notes.txt"              "skips non-image files"
# collision: identical basenames in different dirs stay distinct
assert_contains "albums/2024/photo.jpg"  "disambiguates duplicate filenames (2024)"
assert_contains "albums/2025/photo.jpg"  "disambiguates duplicate filenames (2025)"

# structural properties of the whole listing
TOTAL=$(printf '%s\n' "$ENTRIES" | grep -c '^- ' || true)
UNIQ=$(printf '%s\n' "$ENTRIES" | LC_ALL=C sort -u | grep -c '^- ' || true)
SORTED=$(printf '%s\n' "$ENTRIES" | LC_ALL=C sort)
assert_eq "$TOTAL"   "8"        "matched exactly 8 image files (tied to fixture set)"
assert_eq "$TOTAL"   "$UNIQ"    "no duplicate entries"
assert_eq "$ENTRIES" "$SORTED"  "output is deterministically sorted"

echo "== summary =="
echo "  $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
