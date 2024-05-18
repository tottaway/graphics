#pragma once
#include "model/game_state.hh"

namespace wiz {
class Player : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_player";
  Player(model::GameState &game_state);

  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Result<void, std::string> late_update() { return Ok(); }

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_release(const view::KeyReleasedEvent &key_release);

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  static constexpr std::string_view player_texture_set_path{
      "sprites/wiz/player/player_sprites.yaml"};
  Eigen::Vector2f position_{0.f, 0.f};
  Eigen::Vector2i x_direction_{0, 0};
  Eigen::Vector2i y_direction_{0, 0};
};
} // namespace wiz
