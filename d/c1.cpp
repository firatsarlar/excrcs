#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

// Lowercase a copy of the string for case-insensitive extension matching.
static std::string to_lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

// Recursively lists image files under a root, sorted, for portfolio ingestion.
// results_ is computed once by scan() and shared (by const&) with every
// consumer.
class AssetScraper {
public:
  explicit AssetScraper(fs::path root) : root_(std::move(root)) {}

  // Walk root, keep image files, sort. Populates results_ once; returns count.
  std::size_t scan() {
    results_.clear();
    for (const auto &entry : fs::recursive_directory_iterator(
             root_, fs::directory_options::skip_permission_denied)) {
      if (!entry.is_regular_file() || !is_image(entry.path()))
        continue; // also skips dirs named like images

      results_.push_back(relative_to(entry.path(), root_));
    }
    std::sort(results_.begin(), results_.end()); // deterministic, conflict-free
    return results_.size();
  }

  // Shared access to the single results vector (no copy).
  const std::vector<std::string> &paths() const noexcept { return results_; }

  // Human-formatted listing.
  void print(std::ostream &os) const {
    for (const auto &r : results_)
      os << "- " << r << "\n";
  }

  // Clean path-per-line file for ingestion as an array elsewhere.
  // Returns false if the file cannot be opened.
  bool write(const fs::path &out) const {
    std::error_code ec;
    if (out.has_parent_path())
      fs::create_directories(out.parent_path(), ec);
    std::ofstream os(out);
    if (!os)
      return false;
    for (const auto &r : results_)
      os << r << '\n';
    return true;
  }

private:
  // Case-insensitive image-extension check.
  static bool is_image(const fs::path &p) {
    const std::string ext = to_lower(p.extension().string());
    return std::find(kImageExts.begin(), kImageExts.end(), ext) !=
           kImageExts.end();
  }

  // Path relative to base with '/' separators (stable cross-platform output);
  // keeps same-named files in different folders distinct.
  static std::string relative_to(const fs::path &p, const fs::path &base) {
    return p.lexically_relative(base).generic_string();
  }

  static constexpr std::array<std::string_view, 6> kImageExts = {
      ".jpg", ".jpeg", ".png", ".webp", ".avif", ".svg"};

  fs::path root_;
  std::vector<std::string> results_;
};
constexpr auto kDivider = "-----------------------------------------\n";
constexpr auto kTitle = "RAW FILE LIST GENERATED FOR LLM PROCESSING:\n";
constexpr auto kErrorNotDirectory =
    "Error: Provided path is not a valid directory.\n";
constexpr auto kFilesystemErrorPrefix = "Filesystem Error: ";
constexpr auto kOutputFile = "dist/images.txt";
constexpr auto kErrorWriteFailed =
    "Error: could not open output file for writing: ";
int main() {

  const fs::path root = "."; // current working directory

  std::cout << kDivider << kTitle << kDivider;

  AssetScraper scraper(root);
  try {
    if (!fs::is_directory(root)) {
      std::cerr << kErrorNotDirectory;
      return 1;
    }
    scraper.scan();
  } catch (const fs::filesystem_error &e) {
    std::cerr << kFilesystemErrorPrefix << e.what() << "\n";
    return 1;
  }

  scraper.print(std::cout); // reads results_
  std::cout << kDivider;

  if (!scraper.write(kOutputFile)) { // reads the SAME results_
    std::cerr << kErrorWriteFailed << kOutputFile << "\n";
    return 1;
  }

  std::cout << scraper.paths().size() << " path(s) written to " << kOutputFile
            << "\n";
  return 0;
}
