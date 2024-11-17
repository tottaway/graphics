#pragma once
#include "components/component.hh"
#include "view/screen.hh"
#include "view/texture.hh"

namespace component {
class Sprite : public Component {
public:
  static constexpr std::string_view component_type_name =
      "draw_sprite_component";
  struct SpriteInfo {
    Eigen::Affine2f transform;
    view::Texture texture;
    float z_level = 0;
  };

  using GetSpriteInfoFunc = std::function<SpriteInfo()>;
  Sprite(GetSpriteInfoFunc get_info);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  GetSpriteInfoFunc get_info_;
};
} // namespace component
