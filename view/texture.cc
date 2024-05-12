#include "view/texture.hh"
#include <SFML/Graphics/Rect.hpp>

namespace view {
Texture::Texture(const std::filesystem::path &path) {
  texture_.loadFromFile(path);
}

Texture::Texture(const std::filesystem::path &path,
                 const Eigen::Vector2i &bottom_left,
                 const Eigen::Vector2i &top_right) {
  texture_.loadFromFile(path, sf::IntRect(bottom_left.x(), bottom_left.y(),
                                          top_right.x(), top_right.y()));
}
} // namespace view
