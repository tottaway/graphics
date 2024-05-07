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
  /// @param[in] timestamp_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string>
  update(const float timestamp_s);

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

class SnakeBoard : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_board";
  static constexpr float grid_cell_size_pixels = 0.05f;
  SnakeBoard(model::GameState &game_state);

  [[nodiscard]] Result<void, std::string> init();

  void game_over();

  [[nodiscard]] Result<void, std::string> ate_apple();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] static view::Position get_random_cell_position();

  std::optional<GameResult> maybe_result;

private:
  Result<void, std::string> add_snake();
  Result<void, std::string> add_apple();
  Result<void, std::string> remove_apple();
};

class SnakeBodyElement;

class SnakeHead : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_head";
  SnakeHead(model::GameState &game_state);

  Result<void, std::string> init();

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const;

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  [[nodiscard]] virtual Result<void, std::string>
  update(const float timestamp_s);

  [[nodiscard]] virtual Result<bool, std::string>
  on_key_press(const view::KeyPressedEvent &key_press);

private:
  [[nodiscard]] Result<void, std::string> move_snake();
  bool extend_on_next_move_{false};
  view::Position position_;
  Eigen::Vector2f direction_{1., 0.};
  std::deque<model::EntityID> body_;
};

class SnakeBodyElement : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "snake_body_element";
  SnakeBodyElement(model::GameState &game_state);

  void init(const view::Position &position);

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const;

  [[nodiscard]] virtual std::optional<view::Box> get_bounding_box() const {
    // TODO
    return std::nullopt;
  }

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  view::Position position_;
};

class Apple : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "apple";
  Apple(model::GameState &game_state);

  void init();

  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const;

  [[nodiscard]] virtual std::optional<view::Box> get_bounding_box() const {
    return std::nullopt;
  };

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

private:
  friend SnakeHead;
  view::Position position_;
};

} // namespace snake
