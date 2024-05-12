#pragma once
#include <Eigen/Dense>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>

namespace view {
class Screen;

class Texture {
public:
  Texture(const std::filesystem::path &path);

  Texture(const std::filesystem::path &path, const Eigen::Vector2i &bottom_left,
          const Eigen::Vector2i &top_right);

private:
  friend class Screen;
  sf::Texture texture_;
};
} // namespace view
