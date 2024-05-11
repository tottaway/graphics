#include "tic/tic.hh"
#include "components/draw_rectangle.hh"
#include "components/label.hh"
#include "geometry/transform_utils.hh"
#include "model/rectangle.hh"
#include "utility/overload.hh"
#include "view/screen.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <ranges>
#include <set>

namespace tic {
Result<std::unique_ptr<model::GameState>, std::string> make_tic_game() {
  auto game_state = std::make_unique<model::GameState>();
  TRY(game_state->add_entity(
      std::make_unique<TicGameModeManager>(*game_state)));
  return Ok(std::move(game_state));
}

TicGameModeManager::TicGameModeManager(model::GameState &game_state)
    : Entity(game_state) {}

Result<void, std::string>
TicGameModeManager::update(const int64_t delta_time_ns) {
  switch (game_mode_) {
  case GameMode::init: {
    game_mode_ = GameMode::in_game;
    return start_new_game();
    break;
  }
  case GameMode::in_game: {
    const auto tic_board =
        TRY(game_state_.get_entity_pointer_by_type<TicBoard>());
    const auto maybe_result = tic_board->maybe_result;
    if (maybe_result) {
      remove_child_entities();
      TRY_VOID(add_end_screen(maybe_result.value()));
      game_mode_ = GameMode::end_screen;
    }
    break;
  }
  case GameMode::end_screen: {
    const auto end_screen =
        TRY(game_state_.get_entity_pointer_by_type<EndGame>());

    if (end_screen->has_been_clicked_) {
      remove_child_entities();
      TRY_VOID(start_new_game());
      game_mode_ = GameMode::in_game;
    }
    break;
  }
  }
  return Ok();
}

Result<void, std::string>
TicGameModeManager::add_end_screen(const GameResult &result) {
  TRY(add_child_entity<EndGame>(result));
  return Ok();
}

Result<void, std::string> TicGameModeManager::start_new_game() {
  auto tic_board = TRY(add_child_entity<TicBoard>());
  return tic_board->init();
}

EndGame::EndGame(model::GameState &game_state) : Entity(game_state) {}

void EndGame::init(const GameResult &result) {
  switch (result) {
  case GameResult::x: {
    display_text_ = x_win;
    break;
  }
  case GameResult::o: {
    display_text_ = o_win;
    break;
  }
  case GameResult::tie: {
    display_text_ = tie_game;
    break;
  }
  }
  add_component<component::Label>([this]() {
    return component::Label::TextInfo{display_text_, text_color, font_size,
                                      transform_};
  });
  return Ok();
}

Result<bool, std::string>
EndGame::on_click(const view::MouseUpEvent &mouse_up) {
  has_been_clicked_ = true;
  return Ok(false);
}

Eigen::Affine2f EndGame::get_transform() const { return transform_; }

TicBoard::TicBoard(model::GameState &game_state) : Entity(game_state) {}

Result<void, std::string> TicBoard::init() {
  TRY_VOID(add_tic_squares());
  return add_border_lines();
}

void TicBoard::update_result() {
  // result already set no need to update
  if (maybe_result) {
    return;
  }

  const auto squares = game_state_.get_entity_pointers_by_type<TicSquare>();
  assert(squares.size() == 9UL);

  static const std::vector<std::set<std::size_t>> winning_sets = {
      {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6},
      {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

  std::set<std::size_t> x_indices;
  std::set<std::size_t> o_indices;
  for (const auto [index, square] : std::ranges::views::enumerate(squares)) {
    if (square->state_ == TicSquare::State::x) {
      x_indices.emplace(index);
    } else if (square->state_ == TicSquare::State::o) {
      o_indices.emplace(index);
    }
  }

  for (const auto &winning_set : winning_sets) {
    if (std::ranges::includes(x_indices, winning_set)) {
      maybe_result = GameResult::x;
    } else if (std::ranges::includes(o_indices, winning_set)) {
      maybe_result = GameResult::o;
    }
  }
  if (x_indices.size() + o_indices.size() == squares.size()) {
    maybe_result = GameResult::tie;
  }
}

Result<void, std::string> TicBoard::add_border_lines() {
  const auto add_line =
      [this](const Eigen::Affine2f &transform) -> Result<void, std::string> {
    TRY(add_child_entity<model::StaticDrawnRectangle>(
        transform, view::Color{255, 255, 255}));
    return Ok();
  };

  TRY_VOID(add_line(
      geometry::transform_from_translation_and_scale({0.0, 0.2}, {0.6, 0.01})));
  TRY_VOID(add_line(geometry::transform_from_translation_and_scale(
      {0.0, -0.2}, {0.6, 0.01})));
  TRY_VOID(add_line(
      geometry::transform_from_translation_and_scale({0.2, 0.0}, {0.01, 0.6})));
  return add_line(
      geometry::transform_from_translation_and_scale({-0.2, 0.0}, {0.01, 0.6}));
}

Result<void, std::string> TicBoard::add_tic_squares() {
  const auto add_square =
      [this](const float relative_x,
             const float relative_y) -> Result<void, std::string> {
    TRY(add_child_entity<TicSquare>(
        geometry::transform_from_translation_and_scale({relative_x, relative_y},
                                                       0.2)));
    return Ok();
  };
  TRY_VOID(add_square(-0.4, -0.4));
  TRY_VOID(add_square(-0.4, -0.0));
  TRY_VOID(add_square(-0.4, 0.4));

  TRY_VOID(add_square(0.0, -0.4));
  TRY_VOID(add_square(0.0, -0.0));
  TRY_VOID(add_square(0.0, 0.4));

  TRY_VOID(add_square(0.4, -0.4));
  TRY_VOID(add_square(0.4, -0.0));
  return add_square(0.4, 0.4);
}

TicSquare::TicSquare(model::GameState &game_state) : Entity(game_state) {}

void TicSquare::init(const Eigen::Affine2f &transform) {
  transform_ = transform;
  add_component<component::Label>([this]() { return get_label_info(); });
}

component::Label::TextInfo TicSquare::get_label_info() {
  std::string_view text;
  switch (state_) {
  case State::x:
    text = x_text;
    break;
  case State::o:
    text = o_text;
    break;
  case State::empty:
    text = empty_text;
    break;
  }

  return component::Label::TextInfo{
      text, text_color, font_size,
      get_transform().pretranslate(Eigen::Vector2f{-0.1, 0.1})};
}

Eigen::Affine2f TicSquare::get_transform() const { return transform_; }

Result<bool, std::string>
TicSquare::on_click(const view::MouseUpEvent &mouse_up) {
  if (mouse_up.button != view::MouseButton::Left) {
    return Ok(true);
  }

  switch (state_) {
  case State::x:
  case State::o:
    break;
  case State::empty: {
    const auto tic_board = TRY(get_parent_entity<TicBoard>());
    switch (tic_board->current_turn_player_) {
    case Player::x: {
      tic_board->current_turn_player_ = Player::o;
      state_ = State::x;
      break;
    }
    case Player::o: {
      tic_board->current_turn_player_ = Player::x;
      state_ = State::o;
      break;
    }
    }
    tic_board->update_result();
    break;
  }
  }
  return Ok(false);
}
} // namespace tic
