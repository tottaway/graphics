#pragma once
#include "model/game_state.hh"
#include "view/texture.hh"
#include "wiz/character_mode.hh"

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

  [[nodiscard]] virtual Result<void, std::string> late_update();

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_release(const view::KeyReleasedEvent &key_release);

  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_up(const view::MouseUpEvent &mouse_up) final;

  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_down(const view::MouseDownEvent &mouse_down) final;

  [[nodiscard]] virtual Result<bool, std::string>
  on_mouse_moved(const view::MouseMovedEvent &mouse_moved) final;

  [[nodiscard]] virtual bool get_handle_mouse_events_outside_entitiy() {
    return true;
  }

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual Eigen::Affine2f get_hurt_box_transform() const;

  [[nodiscard]] virtual Eigen::Affine2f get_hit_box_transform() const;

  [[nodiscard]] virtual Eigen::Affine2f get_animation_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

  [[nodiscard]] virtual CharacterMode get_mode() const { return mode_; }

  Eigen::Vector2f position{0.15f, 0.15f};  // Inside border walls (tile 1,1)

  int32_t hp{10};

private:
  static constexpr std::string_view player_texture_set_path{
      "sprites/wiz/player/player_sprites.yaml"};
  void update_mode();

  void set_mode(const CharacterMode mode, const bool init = false);

  bool was_hit_{false};
  bool attacking_{false};
  Eigen::Vector2f attacking_dir_{};

  CharacterMode mode_{CharacterMode::idle};
  Eigen::Vector2i x_direction_{0, 0};
  Eigen::Vector2i y_direction_{0, 0};

  int64_t duration_in_current_mode_ns_{0L};
  int64_t max_duration_in_being_hit_ns_{400'000'000L};
  int64_t max_duration_in_dying_ns_{500'000'000};
  int64_t cool_down_after_hit_ns_{400'000'000L};
  int64_t duration_since_last_exit_hit_ns_{};
};
} // namespace wiz
