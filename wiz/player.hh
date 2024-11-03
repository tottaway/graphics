#pragma once
#include "model/game_state.hh"
#include "view/texture.hh"

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

  Eigen::Vector2f position{0.f, 0.f};

private:
  static constexpr std::string_view player_texture_set_path{
      "sprites/wiz/player/player_sprites.yaml"};
  enum class Mode {
    idle,
    walking_left,
    walking_right,
  };

  void update_mode();

  void set_mode(const Mode mode, const bool init = false);

  Mode mode_{Mode::idle};
  Eigen::Vector2i x_direction_{0, 0};
  Eigen::Vector2i y_direction_{0, 0};

  std::vector<view::Texture> idle_textures_;
  std::vector<view::Texture> walk_right_textures_;
  std::vector<view::Texture> walk_left_textures_;
};
} // namespace wiz
