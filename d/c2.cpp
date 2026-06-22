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

// Fluent image list: scrape once, then chain transforms and sinks.
//   Images::from(".").sort().print().save("dist/images.txt");
class Images {
public:
  static Images from(fs::path root) {
    Images img;
    img.collect(std::move(root));
    return img;
  }

  Images &sort() {
    std::ranges::sort(items_);
    return *this;
  }

  template <class F> Images &each(F &&fn) {
    for (auto &p : items_)
      fn(p);
    return *this;
  }

  Images &print(std::ostream &os = std::cout) {
    for (auto &p : items_)
      os << "- " << p << '\n';
    return *this;
  }

  Images &save(const fs::path &file) {
    std::error_code ec;
    if (file.has_parent_path())
      fs::create_directories(file.parent_path(), ec);
    std::ofstream os(file);
    ok_ = os.is_open();
    if (ok_)
      for (auto &p : items_)
        os << p << '\n';
    return *this;
  }

  const std::vector<std::string> &items() const noexcept { return items_; }
  std::size_t size() const noexcept { return items_.size(); }
  bool ok() const noexcept { return ok_; }

private:
  void collect(fs::path root) {
    if (!fs::is_directory(root))
      return;
    constexpr auto skip = fs::directory_options::skip_permission_denied;
    for (const auto &e : fs::recursive_directory_iterator(root, skip))
      if (e.is_regular_file() && is_image(e.path()))
        items_.push_back(e.path().lexically_relative(root).generic_string());
  }

  static bool is_image(const fs::path &p) {
    auto ext = p.extension().string();
    std::ranges::for_each(ext, [](char &c) {
      c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    });
    return std::ranges::any_of(kExts,
                               [&](std::string_view e) { return e == ext; });
  }

  static constexpr std::array<std::string_view, 6> kExts = {
      ".jpg", ".jpeg", ".png", ".webp", ".avif", ".svg"};

  std::vector<std::string> items_;
  bool ok_ = true;
};

int main() {
  auto imgs = Images::from(".").sort().print().save("dist/images.txt");
  if (!imgs.ok())
    std::cerr << "write failed: dist/images.txt\n";
  return imgs.ok() ? 0 : 1;
}
