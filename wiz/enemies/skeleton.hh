#pragma once
#include "model/game_state.hh"

namespace wiz {
class Skeleton : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_player";
  Skeleton(model::GameState &game_state);

  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Result<void, std::string> late_update() { return Ok(); }

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  static constexpr std::string_view player_texture_set_path{
      "sprites/wiz/skeleton/sprites.yaml"};
  Eigen::Vector2f position_{0.5f, 1.f};
  Eigen::Vector2f x_direction_{0.5f, 0.f};
  Eigen::Vector2f y_direction_{0.f, 0.f};
};
} // namespace wiz
