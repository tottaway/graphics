#pragma once
#include "model/game_state.hh"
#include "view/texture.hh"

namespace wiz {
class Player : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_player";

  enum class Mode {
    idle,
    walking_left,
    walking_right,
    being_hit,
    dying,
    dead,
  };

  Player(model::GameState &game_state);

  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Result<void, std::string> late_update();

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_release(const view::KeyReleasedEvent &key_release);

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

  [[nodiscard]] virtual Mode get_mode() const { return mode_; }

  Eigen::Vector2f position{0.f, 0.f};

  int32_t hp{3};

private:
  static constexpr std::string_view player_texture_set_path{
      "sprites/wiz/player/player_sprites.yaml"};
  void update_mode();

  void set_mode(const Mode mode, const bool init = false);

  bool was_hit_{false};

  Mode mode_{Mode::idle};
  Eigen::Vector2i x_direction_{0, 0};
  Eigen::Vector2i y_direction_{0, 0};

  std::vector<view::Texture> idle_textures_;
  std::vector<view::Texture> walk_right_textures_;
  std::vector<view::Texture> walk_left_textures_;
  std::vector<view::Texture> hit_textures_;
  std::vector<view::Texture> dead_textures_;
  int64_t duration_in_current_mode_ns_{0L};
  int64_t max_duration_in_being_hit_ns_{500'000'000L};
  int64_t max_duration_in_dying_ns_{500'000'000L};
  int64_t cool_down_after_hit_ns_{500'000'000L};
  int64_t duration_since_last_exit_hit_ns_{};
};
} // namespace wiz
