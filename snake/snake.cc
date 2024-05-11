#include "snake/snake.hh"
#include "components/draw_grid_cell.hh"
#include "components/grid_collider.hh"
#include "components/label.hh"
#include "model/rectangle.hh"
#include "systems/grid_collisions.hh"
#include "view/screen.hh"
#include <SFML/Window/Keyboard.hpp>

namespace snake {
namespace {
/// TODO this should share code with the draw grid cell component
Eigen::Affine2f transform_from_grid_cell(const Eigen::Vector2i grid_cell) {
  // divide by two because rectangle have a max side length of 2
  return Eigen::Affine2f(
             Eigen::Translation2f{grid_cell.cast<float>() *
                                  SnakeBoard::get_grid_cell_size_pixels()})
      .scale(SnakeBoard::get_grid_cell_size_pixels() / 2);
}
} // namespace
Result<std::unique_ptr<model::GameState>, std::string> make_snake_game() {
  auto game_state = std::make_unique<model::GameState>();
  // TODO pull dims from source
  game_state->add_system<systems::GridCollisions<42, 42>>();
  TRY(game_state->add_entity(std::make_unique<SnakeModeManager>(*game_state)));
  return Ok(std::move(game_state));
}

SnakeModeManager::SnakeModeManager(model::GameState &game_state)
    : Entity(game_state) {}

Result<void, std::string> SnakeModeManager::update(const int64_t) {
  switch (game_mode_) {
  case GameMode::init: {
    TRY_VOID(start_new_game());
    game_mode_ = GameMode::in_game;
    break;
  }
  case GameMode::in_game: {
    const auto snake_board =
        TRY(game_state_.get_entity_pointer_by_type<SnakeBoard>());
    if (snake_board->maybe_result) {
      remove_child_entities();
      TRY(add_child_entity_and_init<EndScreen>());
      game_mode_ = GameMode::end_screen;
    }
    break;
  }
  case GameMode::end_screen: {
    const auto end_screen =
        TRY(game_state_.get_entity_pointer_by_type<EndScreen>());
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

EndScreen::EndScreen(model::GameState &game_state) : Entity(game_state) {}

void EndScreen::init() {
  add_component<component::Label>([this]() {
    return component::Label::TextInfo{text, text_color, font_size, transform_};
  });
}

Result<bool, std::string>
EndScreen::on_click(const view::MouseUpEvent &mouse_up) {
  has_been_clicked_ = true;
  return Ok(false);
}

Result<void, std::string> SnakeModeManager::start_new_game() {
  TRY(add_child_entity_and_init<SnakeBoard>());
  return Ok();
}

SnakeBoard::SnakeBoard(model::GameState &game_state) : Entity(game_state) {}

Result<void, std::string> SnakeBoard::init() {
  TRY(add_child_entity_and_init<model::StaticDrawnRectangle>(
      get_transform(), view::Color{120, 120, 120}));
  TRY(add_child_entity_and_init<SnakeHead>());
  TRY(add_child_entity_and_init<Apple>());
  return Ok();
}

Eigen::Vector2i SnakeBoard::get_random_cell_position() {
  // TODO don't generate every time
  std::random_device rd_{}; // obtain a random number from hardware
  std::mt19937 gen(rd_());  // seed the generator
  const auto bounds = get_grid_size();
  const auto max_x = bounds.x() - 1;
  const auto max_y = bounds.y() - 1;
  std::uniform_int_distribution<> x_distr(-max_x, max_x);
  std::uniform_int_distribution<> y_distr(-max_y, max_y);
  return {x_distr(gen), y_distr(gen)};
}

Result<void, std::string> SnakeBoard::ate_apple() {
  game_state_.remove_entities_by_type<Apple>();
  TRY(add_child_entity_and_init<Apple>());
  return Ok();
}

void SnakeBoard::game_over() { maybe_result = GameResult{10}; }

SnakeHead::SnakeHead(model::GameState &game_state) : Entity(game_state) {}

Result<void, std::string> SnakeHead::init() {
  current_cell_ = SnakeBoard::get_random_cell_position();
  // add first body element
  auto snake_body1 = TRY(
      add_child_entity_and_init<SnakeBodyElement>(current_cell_ - direction_));
  body_.emplace_back(snake_body1->get_entity_id());

  // add a second
  auto snake_body2 = TRY(add_child_entity_and_init<SnakeBodyElement>(
      current_cell_ - direction_ * 2));
  body_.emplace_back(snake_body2->get_entity_id());

  add_component<component::DrawGridCell>(
      SnakeBoard::get_grid_size(), SnakeBoard::get_grid_cell_size_pixels(),
      [this]() {
        return component::DrawGridCell::CellInfo{current_cell_, head_color};
      });

  add_component<component::GridCollider>(
      [this]() {
        std::vector<Eigen::Vector2i> result = {current_cell_};
        return result;
      },
      [this](const model::EntityID entity_id) {
        collided_entities_.emplace_back(entity_id);
      });
  return Ok();
}

Result<void, std::string> SnakeHead::move_snake() {
  // draw body where the head is now
  auto new_snake_body =
      TRY(add_child_entity_and_init<SnakeBodyElement>(current_cell_));
  body_.emplace_front(new_snake_body->get_entity_id());

  if (!extend_on_next_move_) {
    const auto tail_id = body_.back();
    remove_entity(tail_id);
    body_.pop_back();
  }
  extend_on_next_move_ = false;

  // move head
  current_cell_ += direction_;
  if (current_cell_.x() > SnakeBoard::get_grid_size().x() ||
      current_cell_.y() > SnakeBoard::get_grid_size().y() ||
      current_cell_.x() < -SnakeBoard::get_grid_size().x() ||
      current_cell_.y() < -SnakeBoard::get_grid_size().y()) {
    const auto snake_board = TRY(get_parent_entity<SnakeBoard>());
    snake_board->game_over();
  }
  return Ok();
}

Result<bool, std::string>
SnakeHead::on_key_press(const view::KeyPressedEvent &key_press) {
  if (key_pressed_this_update_) {
    return Ok(false);
  }

  if (key_press.key_event.code == sf::Keyboard::W && (direction_.y() == 0)) {
    direction_ = {0, 1};
    key_pressed_this_update_ = true;
    return Ok(false);
  } else if (key_press.key_event.code == sf::Keyboard::A &&
             (direction_.x() == 0)) {
    direction_ = {-1, 0};
    key_pressed_this_update_ = true;
    return Ok(false);
  } else if (key_press.key_event.code == sf::Keyboard::S &&
             (direction_.y() == 0)) {
    direction_ = {0, -1};
    key_pressed_this_update_ = true;
    return Ok(false);
  } else if (key_press.key_event.code == sf::Keyboard::D &&
             (direction_.x() == 0)) {
    direction_ = {1, 0};
    key_pressed_this_update_ = true;
    return Ok(false);
  } else {
    return Ok(true);
  }
}

Result<void, std::string> SnakeHead::update(const int64_t delta_time_ns) {
  TRY_VOID(move_snake());
  key_pressed_this_update_ = false;
  return Ok();
}

Result<void, std::string> SnakeHead::late_update() {
  for (const auto entity_id : collided_entities_) {
    const auto maybe_entity =
        game_state_.try_get_entity_pointer_by_id(entity_id);
    if (!maybe_entity) {
      continue;
    }

    const auto snake_board = TRY(get_parent_entity<SnakeBoard>());
    if (maybe_entity.value()->is<Apple>()) {
      TRY_VOID(snake_board->ate_apple());
      extend_on_next_move_ = true;
    } else if (maybe_entity.value()->is<SnakeBodyElement>()) {
      snake_board->game_over();
    }
  }
  return Ok();
}

Eigen::Affine2f SnakeHead::get_transform() const {
  return transform_from_grid_cell(current_cell_);
}

SnakeBodyElement::SnakeBodyElement(model::GameState &game_state)
    : Entity(game_state) {}

void SnakeBodyElement::init(const Eigen::Vector2i &current_cell) {
  current_cell_ = current_cell;
  add_component<component::DrawGridCell>(
      SnakeBoard::get_grid_size(), SnakeBoard::get_grid_cell_size_pixels(),
      [this]() {
        return component::DrawGridCell::CellInfo{current_cell_, body_color};
      });

  add_component<component::GridCollider>(
      [this]() {
        std::vector<Eigen::Vector2i> result = {current_cell_};
        return result;
      },
      [this](const model::EntityID entity_id) {
        const auto maybe_snake_head =
            game_state_.get_entity_pointer_by_id_as<SnakeHead>(entity_id);
        if (maybe_snake_head.isOk()) {
          maybe_snake_head.unwrap()->collided_entities_.emplace_back(
              get_entity_id());
        }
      });
}

Eigen::Affine2f SnakeBodyElement::get_transform() const {
  return transform_from_grid_cell(current_cell_);
}

Apple::Apple(model::GameState &game_state) : Entity(game_state) {}

void Apple::init() {
  current_cell_ = SnakeBoard::get_random_cell_position();
  add_component<component::DrawGridCell>(
      SnakeBoard::get_grid_size(), SnakeBoard::get_grid_cell_size_pixels(),
      [this]() {
        return component::DrawGridCell::CellInfo{current_cell_, apple_color};
      });

  add_component<component::GridCollider>(
      [this]() {
        std::vector<Eigen::Vector2i> result = {current_cell_};
        return result;
      },
      [this](const model::EntityID entity_id) {
        const auto maybe_snake_head =
            game_state_.get_entity_pointer_by_id_as<SnakeHead>(entity_id);
        if (maybe_snake_head.isOk()) {
          maybe_snake_head.unwrap()->collided_entities_.emplace_back(
              get_entity_id());
        }
      });
}

Eigen::Affine2f Apple::get_transform() const {
  return transform_from_grid_cell(current_cell_);
}
} // namespace snake
