#include "snake/snake.hh"
#include "view/screen.hh"
#include <SFML/Window/Keyboard.hpp>

namespace snake {
Result<std::unique_ptr<model::GameState>, std::string> make_snake_game() {
  auto game_state = std::make_unique<model::GameState>();
  TRY(game_state->add_entity(std::make_unique<SnakeModeManager>(*game_state)));
  return Ok(std::move(game_state));
}

SnakeModeManager::SnakeModeManager(model::GameState &game_state)
    : Entity(game_state) {}

Result<void, std::string> SnakeModeManager::update(const float) {
  switch (game_mode_) {
  case GameMode::init:
    TRY_VOID(start_new_game());
    game_mode_ = GameMode::in_game;
    break;
  case GameMode::in_game:
    break;
  case GameMode::end_screen:
    break;
  }
  return Ok();
}

Result<void, std::string> SnakeModeManager::start_new_game() {
  auto board = TRY(add_child_entity<SnakeBoard>());
  return board->init();
}

SnakeBoard::SnakeBoard(model::GameState &game_state) : Entity(game_state) {}

Result<void, std::string> SnakeBoard::init() {
  auto snake_head = TRY(add_child_entity<SnakeHead>());
  TRY_VOID(snake_head->init());

  auto apple = TRY(add_child_entity<Apple>());
  apple->init();
  return Ok();
}

view::Position SnakeBoard::get_random_cell_position() {
  // TODO don't generate every time
  std::random_device rd_{}; // obtain a random number from hardware
  std::mt19937 gen(rd_());  // seed the generator
  std::uniform_int_distribution<> distr(0, 20); // define the range
  return view::make_relative_position(
      distr(gen) * SnakeBoard::grid_cell_size_pixels,
      distr(gen) * SnakeBoard::grid_cell_size_pixels);
}

Result<void, std::string> SnakeBoard::ate_apple() {
  game_state_.remove_entities_by_type<Apple>();
  auto apple = TRY(add_child_entity<Apple>());
  apple->init();
  return Ok();
}

SnakeHead::SnakeHead(model::GameState &game_state) : Entity(game_state) {}

Result<void, std::string> SnakeHead::init() {
  position_ = SnakeBoard::get_random_cell_position();
  // add first body element
  auto snake_body1 = TRY(add_child_entity<SnakeBodyElement>());
  snake_body1->init(view::make_relative_position(
      position_.coord.vec - direction_ * SnakeBoard::grid_cell_size_pixels));
  body_.emplace_back(snake_body1->get_entity_id());

  // add a second
  auto snake_body2 = TRY(add_child_entity<SnakeBodyElement>());
  snake_body2->init(view::make_relative_position(
      position_.coord.vec -
      direction_ * SnakeBoard::grid_cell_size_pixels * 2));
  body_.emplace_back(snake_body2->get_entity_id());
  return Ok();
}

Result<void, std::string> SnakeHead::move_snake() {
  // draw body where the head is now
  auto new_snake_body = TRY(add_child_entity<SnakeBodyElement>());
  new_snake_body->init(view::make_relative_position(position_.coord.vec));
  body_.emplace_front(new_snake_body->get_entity_id());

  if (!extend_on_next_move_) {
    const auto tail_id = body_.back();
    remove_entity(tail_id);
    body_.pop_back();
  }
  extend_on_next_move_ = false;

  // move head
  position_.coord.vec += direction_ * SnakeBoard::grid_cell_size_pixels;
  return Ok();
}

Result<bool, std::string>
SnakeHead::on_key_press(const view::KeyPressedEvent &key_press) {
  if (key_press.key_event.code == sf::Keyboard::W && !(direction_.y() > 0)) {
    direction_ = {0.f, -1.f};
    return Ok(false);
  } else if (key_press.key_event.code == sf::Keyboard::A &&
             !(direction_.x() > 0)) {
    direction_ = {-1.f, 0.f};
    return Ok(false);
  } else if (key_press.key_event.code == sf::Keyboard::S &&
             !(direction_.y() < 0)) {
    direction_ = {0.f, 1.f};
    return Ok(false);
  } else if (key_press.key_event.code == sf::Keyboard::D &&
             !(direction_.x() < 0)) {
    direction_ = {1.f, 0.f};
    return Ok(false);
  } else {
    return Ok(true);
  }
}

Result<void, std::string> SnakeHead::update(const int64_t delta_time_ns) {
  TRY_VOID(move_snake());

  const auto apple = TRY(game_state_.get_entity_pointer_by_type<Apple>());
  // TODO real collisions
  if ((apple->position_.coord.vec - position_.coord.vec).norm() < 1e-5) {
    extend_on_next_move_ = true;
    const auto snake_board = TRY(get_parent_entity<SnakeBoard>());
    TRY_VOID(snake_board->ate_apple());
  }

  return Ok();
}

Result<void, std::string> SnakeHead::draw(view::Screen &screen) const {
  screen.draw_rectangle(
      view::Box{position_,
                view::make_relative_size(SnakeBoard::grid_cell_size_pixels,
                                         SnakeBoard::grid_cell_size_pixels)},
      view::Color{0, 255, 0});
  return Ok();
}

SnakeBodyElement::SnakeBodyElement(model::GameState &game_state)
    : Entity(game_state) {}

void SnakeBodyElement::init(const view::Position &position) {
  position_ = position;
}

Result<void, std::string> SnakeBodyElement::draw(view::Screen &screen) const {
  screen.draw_rectangle(
      view::Box{position_,
                view::make_relative_size(SnakeBoard::grid_cell_size_pixels,
                                         SnakeBoard::grid_cell_size_pixels)},
      view::Color{0, 185, 0});
  return Ok();
}

Apple::Apple(model::GameState &game_state) : Entity(game_state) {}

void Apple::init() { position_ = SnakeBoard::get_random_cell_position(); }

Result<void, std::string> Apple::draw(view::Screen &screen) const {
  screen.draw_rectangle(
      view::Box{position_,
                view::make_relative_size(SnakeBoard::grid_cell_size_pixels,
                                         SnakeBoard::grid_cell_size_pixels)},
      view::Color{255, 0, 0});
  return Ok();
}
} // namespace snake
