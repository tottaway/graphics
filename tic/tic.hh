#pragma once
#include "model/game_state.hh"
#include "view/screen.hh"
#include <Eigen/Dense>

namespace tic {
class TicSquare;

enum class Player { x, o };

enum class GameResult { x, o, tie };

[[nodiscard]] Result<std::unique_ptr<model::GameState>, std::string>
make_tic_game();

class TicGameModeManager : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "tic_mode_manager";

  virtual ~TicGameModeManager() = default;
  TicGameModeManager(model::GameState &game_state);

  /// Update the internal state of the Enitity
  /// @param[in] delta_time_ns the current time in nanoseconds
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns) final;

  [[nodiscard]] virtual std::string_view get_entity_type_name() const final {
    return entity_type_name;
  };

private:
  enum class GameMode { init, in_game, end_screen };
  [[nodiscard]] Result<void, std::string>
  add_end_screen(const GameResult &result);
  [[nodiscard]] Result<void, std::string> start_new_game();
  GameMode game_mode_{GameMode::init};
};

class EndGame : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "tic_end_game";
  virtual ~EndGame() = default;
  EndGame(model::GameState &game_state);

  void init(const GameResult &result);

  /// Draw the entity on the screen
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const final;

  [[nodiscard]] virtual Result<bool, std::string>
  on_click(const view::MouseUpEvent &mouse_up) final;

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const final;

  [[nodiscard]] virtual std::string_view get_entity_type_name() const final {
    return entity_type_name;
  };

  bool has_been_clicked_{false};

private:
  GameResult game_result_;
  Eigen::Affine2f transform_ = Eigen::Affine2f::Identity();
};

class TicBoard : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "tic_board";

  TicBoard(model::GameState &game_state);

  [[nodiscard]] Result<void, std::string> init();

  void update_result();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const final {
    return entity_type_name;
  };

  Player current_turn_player_{Player::x};

  std::optional<GameResult> maybe_result;

private:
  [[nodiscard]] Result<void, std::string> add_border_lines();
  [[nodiscard]] Result<void, std::string> add_tic_squares();
};

class TicSquare : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "tic_square";

  enum class State { x, o, empty };

  virtual ~TicSquare() = default;

  TicSquare(model::GameState &game_state);

  void init(const Eigen::Affine2f &box);

  /// Draw the entity on the screen
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen &screen) const;

  [[nodiscard]] virtual Eigen::Affine2f get_transform() const final;

  [[nodiscard]] virtual Result<bool, std::string>
  on_click(const view::MouseUpEvent &mouse_up);

  [[nodiscard]] virtual std::string_view get_entity_type_name() const {
    return entity_type_name;
  };

  State state_{State::empty};

private:
  static constexpr std::string_view x_text = "X";
  static constexpr std::string_view o_text = "O";
  static constexpr std::string_view empty_text = " ";

  Eigen::Affine2f transform_;
};
} // namespace tic
