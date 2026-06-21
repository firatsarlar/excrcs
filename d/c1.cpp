#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

// Lowercase a copy of the string for case-insensitive extension matching.
static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

int main() {
    // Target directory ("." = current working directory).
    const std::string path = ".";

    std::cout << "-----------------------------------------\n";
    std::cout << "RAW FILE LIST GENERATED FOR LLM PROCESSING:\n";
    std::cout << "-----------------------------------------\n";

    static const std::vector<std::string> kImageExts = {
        ".jpg", ".jpeg", ".png", ".webp", ".avif", ".svg"
    };

    std::vector<std::string> results;

    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (!fs::is_regular_file(entry.status())) continue;

                std::string ext = to_lower(entry.path().extension().string());
                if (std::find(kImageExts.begin(), kImageExts.end(), ext) == kImageExts.end())
                    continue;

                // Relative path keeps same-named files in different folders distinct;
                // generic_string() forces '/' separators for cross-platform output.
                results.push_back(entry.path().lexically_relative(path).generic_string());
            }
        } else {
            std::cerr << "Error: Provided path is not a valid directory.\n";
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem Error: " << e.what() << "\n";
    }

    // Deterministic, reproducible (conflict-free) ordering.
    std::sort(results.begin(), results.end());
    for (const auto& r : results) std::cout << "- " << r << "\n";

    std::cout << "-----------------------------------------\n";
    return 0;
}
