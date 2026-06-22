#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <array>
#include <string_view>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

// Lowercase a copy of the string for case-insensitive extension matching.
static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

// Path relative to base with '/' separators (stable cross-platform output);
// keeps same-named files in different folders distinct.
static std::string relative_path(const fs::path& p, const fs::path& base) {
    return p.lexically_relative(base).generic_string();
}

int main() {
    // Target directory ("." = current working directory).
    const std::string path = ".";

    constexpr auto kDivider = "-----------------------------------------\n";
    constexpr auto kTitle   = "RAW FILE LIST GENERATED FOR LLM PROCESSING:\n";
    constexpr auto kErrorNotDirectory = "Error: Provided path is not a valid directory.\n";
    constexpr auto kFilesystemErrorPrefix = "Filesystem Error: ";
    std::cout << kDivider << kTitle << kDivider;

    static constexpr std::array<std::string_view, 6> kImageExts = {
        ".jpg", ".jpeg", ".png", ".webp", ".avif", ".svg"
    };

    std::vector<std::string> results;

    try {
        if (fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(
                     path, fs::directory_options::skip_permission_denied)) {
                if (!entry.is_regular_file()) continue;

                std::string ext = to_lower(entry.path().extension().string());
                if (std::find(kImageExts.begin(), kImageExts.end(), ext) == kImageExts.end())
                    continue;

                results.push_back(relative_path(entry.path(), path));
            }
        } else {
            std::cerr << kErrorNotDirectory;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << kFilesystemErrorPrefix << e.what() << "\n";
    }

    // Deterministic, reproducible (conflict-free) ordering.
    std::sort(results.begin(), results.end());
    for (const auto& r : results) std::cout << "- " << r << "\n";

    std::cout << kDivider;
    return 0;
}
