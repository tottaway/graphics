#pragma once
#include <Eigen/Dense>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>
#include <unordered_map>

namespace view {
class Screen;

class Texture {
public:
  Texture(const std::filesystem::path &path);

  Texture(const std::filesystem::path &path, const Eigen::Vector2i &bottom_left,
          const Eigen::Vector2i &top_right);

private:
  friend class Screen;
  std::shared_ptr<sf::Texture> texture_;
  Eigen::Vector2f bottom_left_uv_;
  Eigen::Vector2f top_right_uv_;

  static std::unordered_map<std::string, std::shared_ptr<sf::Texture>>
      s_texture_cache;
};
} // namespace view
