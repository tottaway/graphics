#include "view/texture.hh"
#include <SFML/Graphics/Rect.hpp>
#include <iostream>

namespace view {

std::unordered_map<std::string, std::shared_ptr<sf::Texture>>
    Texture::s_texture_cache{};

Texture::Texture(const std::filesystem::path &path) {
  const auto find_result = s_texture_cache.find(path.string());
  if (find_result != s_texture_cache.end()) {
    texture_ = find_result->second;
  } else {
    texture_ = std::make_shared<sf::Texture>();
    texture_->loadFromFile(path);
    s_texture_cache.emplace(path.string(), texture_);
  }
  bottom_left_uv_ = {0.f, 0.f};
  top_right_uv_ = {1.f, 1.f};
}

Texture::Texture(const std::filesystem::path &path,
                 const Eigen::Vector2i &bottom_left,
                 const Eigen::Vector2i &top_right) {
  const auto find_result = s_texture_cache.find(path.string());
  if (find_result != s_texture_cache.end()) {
    texture_ = find_result->second;
  } else {
    texture_ = std::make_shared<sf::Texture>();
    texture_->loadFromFile(path);
    s_texture_cache.emplace(path.string(), texture_);
  }

  const auto size = texture_->getSize();
  const Eigen::Vector2f size_vec{size.x, size.y};
  bottom_left_uv_ = bottom_left.cast<float>().cwiseQuotient(size_vec);
  top_right_uv_ = top_right.cast<float>().cwiseQuotient(size_vec);
}
} // namespace view
