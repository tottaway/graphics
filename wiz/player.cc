#include "wiz/player.hh"
#include "components/animation.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/map/grass_tile.hh"

namespace wiz {
Player::Player(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Player::init() {
  add_component<component::Center>([this]() { return get_transform(); });

  add_component<component::SolidAABBCollider>(
      [this]() { return get_transform(); },
      [this](const Eigen::Vector2f &translation) {
        this->position += translation;
      });

  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(player_texture_set_path)));
  idle_textures_ = texture_set->get_texture_set_by_name("idle");
  walk_right_textures_ = texture_set->get_texture_set_by_name("move_right");
  walk_left_textures_ = texture_set->get_texture_set_by_name("move_left");

  set_mode(Mode::idle, true);

  return Ok();
}

Result<void, std::string> Player::update(const int64_t delta_time_ns) {
  position += (y_direction_ + x_direction_).cast<float>().normalized() *
              (static_cast<double>(delta_time_ns) / 1e9);
  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }
  return Ok();
}

void Player::update_mode() {
  if (x_direction_.norm() > 0 || y_direction_.norm() > 0) {
    if (x_direction_.x() > 0) {
      set_mode(Mode::walking_right);
    } else {
      set_mode(Mode::walking_left);
    }
  } else {
    set_mode(Mode::idle);
  }
}

void Player::set_mode(const Mode mode, const bool init) {
  if (mode == mode_ && !init) {
    return;
  }

  remove_components<component::Animation>();
  mode_ = mode;
  std::vector<view::Texture> textures;
  switch (mode) {
  case Mode::idle: {
    textures = idle_textures_;
    break;
  }
  case Mode::walking_left: {
    textures = walk_left_textures_;
    break;
  }
  case Mode::walking_right: {
    textures = walk_right_textures_;
    break;
  }
  }

  add_component<component::Animation>(
      [this]() {
        return get_transform()
            .scale(Eigen::Vector2f{1.0, 1.3f})
            .translate(Eigen::Vector2f{0.f, 0.17f});
      },
      std::move(textures), 10.f);
}

Result<bool, std::string>
Player::on_key_press(const view::KeyPressedEvent &key_press) {
  if (key_press.key_event.code == sf::Keyboard::W) {
    y_direction_ += Eigen::Vector2i{0, 1};
  } else if (key_press.key_event.code == sf::Keyboard::A) {
    x_direction_ += Eigen::Vector2i{-1, 0};
  } else if (key_press.key_event.code == sf::Keyboard::S) {
    y_direction_ += Eigen::Vector2i{0, -1};
  } else if (key_press.key_event.code == sf::Keyboard::D) {
    x_direction_ += Eigen::Vector2i{1, 0};
  } else {
    return Ok(true);
  }
  x_direction_.normalize();
  y_direction_.normalize();

  update_mode();

  return Ok(false);
}

Result<bool, std::string>
Player::on_key_release(const view::KeyReleasedEvent &key_release) {
  if (key_release.key_event.code == sf::Keyboard::W) {
    y_direction_ -= Eigen::Vector2i{0, 1};
  } else if (key_release.key_event.code == sf::Keyboard::A) {
    x_direction_ -= Eigen::Vector2i{-1, 0};
  } else if (key_release.key_event.code == sf::Keyboard::S) {
    y_direction_ -= Eigen::Vector2i{0, -1};
  } else if (key_release.key_event.code == sf::Keyboard::D) {
    x_direction_ -= Eigen::Vector2i{1, 0};
  } else {
    return Ok(true);
  }

  x_direction_.normalize();
  y_direction_.normalize();

  update_mode();

  return Ok(false);
}

Eigen::Affine2f Player::get_transform() const {
  float x_scale_factor;
  switch (mode_) {
  case Mode::idle: {
    x_scale_factor = 0.07;
    break;
  }

  case Mode::walking_left:
    [[fallthrough]];
  case Mode::walking_right: {
    x_scale_factor = 0.1;
    break;
  }
  }

  return geometry::make_rectangle_from_center_and_size(
      position, Eigen::Vector2f{x_scale_factor, 0.1f});
}
} // namespace wiz
