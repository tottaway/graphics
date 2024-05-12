#include "view/texture.hh"

namespace view {
Texture::Texture(const std::filesystem::path &path) {
  texture_.loadFromFile(path);
}
} // namespace view
