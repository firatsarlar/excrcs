// C++20 modernization of c1.cpp (the C++17 original is kept as-is for comparison).
// Realistic: compiles today on Apple clang / libc++ with -std=c++20.
// Highlights vs c1.cpp: ranges views replace the manual loop, std::ranges
// algorithms, std::format replaces << chains, CTAD on the extension table.
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <algorithm>
#include <ranges>
#include <format>
#include <iostream>
#include <cctype>

namespace fs = std::filesystem;
using namespace std::string_view_literals;

// Compile-time image-extension table (CTAD deduces array<string_view, 6>).
constexpr std::array kImageExts =
    { ".jpg"sv, ".jpeg"sv, ".png"sv, ".webp"sv, ".avif"sv, ".svg"sv };

static bool is_image(const fs::path& p) {
    std::string ext = p.extension().string();
    std::ranges::transform(ext, ext.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    return std::ranges::find(kImageExts, ext) != kImageExts.end();
}

int main() {
    const fs::path root = ".";
    constexpr auto kDivider = "-----------------------------------------\n";

    std::cout << std::format("{0}RAW FILE LIST GENERATED FOR LLM PROCESSING:\n{0}",
                             kDivider);

    std::vector<std::string> results;
    try {
        if (!fs::is_directory(root)) {
            std::cerr << "Error: Provided path is not a valid directory.\n";
            return 1;
        }

        auto entries = fs::recursive_directory_iterator(
            root, fs::directory_options::skip_permission_denied);

        // walk -> keep regular files -> keep images -> relative '/'-separated path
        auto images = entries
            | std::views::filter([](const fs::directory_entry& e) { return e.is_regular_file(); })
            | std::views::filter([](const fs::directory_entry& e) { return is_image(e.path()); })
            | std::views::transform([&](const fs::directory_entry& e) {
                  return e.path().lexically_relative(root).generic_string();
              });

        for (auto&& rel : images) results.push_back(std::move(rel));
    } catch (const fs::filesystem_error& e) {
        std::cerr << std::format("Filesystem Error: {}\n", e.what());
    }

    std::ranges::sort(results);
    for (const auto& r : results) std::cout << std::format("- {}\n", r);

    std::cout << kDivider;
    return 0;
}
