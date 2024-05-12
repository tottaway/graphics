#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>

namespace view {
class Screen;

class Texture {
public:
  Texture(const std::filesystem::path &path);

private:
  friend class Screen;
  sf::Texture texture_;
};
} // namespace view
