#pragma once
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/screen.hh"
#include <Eigen/Dense>
#include <deque>
#include <random>

namespace snake {

struct GameResult {
  uint16_t final_score;
};

[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_snake_game();

class SnakeModeManager : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_mode_manager";

  virtual ~SnakeModeManager() = default;
  SnakeModeManager(model::GameState &game_state);

  /// Update the internal state of the Enitity
  /// @param[in] delta_time_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  enum class GameMode { init, in_game, end_screen };
  [[nodiscard]] Result<void, std::string>
  add_end_screen(const GameResult &result);
  [[nodiscard]] Result<void, std::string> start_new_game();
  [[nodiscard]] Result<void, std::string> clear_board();
  [[nodiscard]] Result<void, std::string> clear_end_screen();
  GameMode game_mode_{GameMode::init};
};

class EndScreen : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_end_screen";
  virtual ~EndScreen() = default;
  EndScreen(model::GameState &game_state);

  void init(const GameResult &game_result);

  [[nodiscard]] virtual Result<bool, std::string>
  on_click(const view::MouseUpEvent &mouse_up) final;

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const final {
    return transform_;
  }

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const final {
    return entity_type_name;
  };

  bool has_been_clicked_{false};

private:
  static constexpr float font_size = 64.f;
  static constexpr view::Color text_color{94, 108, 87};

  Eigen::Affine2f transform_{Eigen::Translation2f{-0.4f, 0.0f}};
  std::string display_text_;
};

class SnakeBoard : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_board";
  static Eigen::Vector2i get_grid_size() { return {20, 20}; }
  static Eigen::Vector2f get_grid_cell_size() { return {0.05f, 0.05f}; }

  SnakeBoard(model::GameState &game_state);

  [[nodiscard]] Result<void, std::string> init();

  void game_over();

  [[nodiscard]] Result<void, std::string> ate_apple();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] static Eigen::Vector2i get_random_cell_position();

  std::optional<GameResult> maybe_result;

private:
  Result<void, std::string> add_snake();
  Result<void, std::string> add_apple();
  Result<void, std::string> remove_apple();

  uint16_t score_{0U};
};

class SnakeBodyElement;

class SnakeHead : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_head";
  SnakeHead(model::GameState &game_state);

  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns);

  [[nodiscard]] virtual Result<void, std::string> late_update();

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  std::vector<model::EntityID> collided_entities_;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  static constexpr view::Color head_color{158, 175, 121};
  [[nodiscard]] Result<void, std::string> move_snake();
  bool extend_on_next_move_{false};
  bool key_pressed_this_update_{false};
  Eigen::Vector2i current_cell_;
  Eigen::Vector2i direction_{1, 0};
  std::deque<model::EntityID> body_;
};

class SnakeBodyElement : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_body_element";
  SnakeBodyElement(model::GameState &game_state);

  void init(const Eigen::Vector2i &current_cell);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  static constexpr view::Color body_color{94, 108, 87};
  Eigen::Vector2i current_cell_;
};

class Apple : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "apple";
  Apple(model::GameState &game_state);

  void init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const;

  [[nodiscard]] virtual uint8_t get_z_level() const { return 1; }

private:
  friend SnakeHead;
  static constexpr view::Color apple_color{150, 46, 75};
  static constexpr std::string_view apple_texture_path{
      "sprites/snake/apple.png"};
  Eigen::Vector2i current_cell_;
};

} // namespace snake
