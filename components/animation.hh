#pragma once
#include "components/sprite.hh"
#include "view/screen.hh"
#include "view/texture.hh"
#include <Eigen/Dense>

namespace component {
class Animation : public Sprite {
public:
  static constexpr std::string_view component_type_name =
      "draw_animation_component";

  using GetTransformFunc = std::function<Eigen::Affine2f()>;
  Animation(GetTransformFunc get_transform, std::vector<view::Texture> textures,
            const float fps);

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] SpriteInfo get_sprite_info();

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  GetTransformFunc get_transform_;
  std::vector<view::Texture> textures_;
  std::size_t current_texture_index_{0UL};
  int64_t nanos_since_last_change_{0L};
  int64_t nanos_between_changes_{0L};
};
} // namespace component
