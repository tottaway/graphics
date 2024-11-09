#include "wiz/player.hh"
#include "components/animation.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/hit_box.hh"
#include "components/hurt_box.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/components/health_bar.hh"
#include "wiz/components/hit_hurt_boxes.hh"
#include "wiz/map/grass_tile.hh"

namespace wiz {
Player::Player(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Player::init() {
  add_component<component::Center>([this]() { return get_transform(); });

  add_component<component::SolidAABBCollider>(
      [this]() { return get_hurt_box_transform(); },
      [this](const Eigen::Vector2f &translation) {
        this->position += translation;
      });

  add_component<WizHurtBox<Alignement::good>>(
      [this]() { return get_hurt_box_transform(); },
      [this]() { was_hit_ = true; });

  add_component<WizHitBox<Alignement::good>>(
      [this]() { return get_hit_box_transform(); });

  add_component<component::DrawRectangle>([this]() {
    return component::DrawRectangle::RectangleInfo{get_hit_box_transform(),
                                                   view::Color{253, 220, 151}};
  });

  add_component<HealthBar>(
      hp, [this]() { return hp; }, [this]() { return get_transform(); });

  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(player_texture_set_path)));
  idle_textures_ = texture_set->get_texture_set_by_name("idle");
  walk_right_textures_ = texture_set->get_texture_set_by_name("move_right");
  walk_left_textures_ = texture_set->get_texture_set_by_name("move_left");
  attack_right_textures_ = texture_set->get_texture_set_by_name("attack_right");
  attack_left_textures_ = texture_set->get_texture_set_by_name("attack_left");
  hit_textures_ = texture_set->get_texture_set_by_name("take_hit");
  dead_textures_ = texture_set->get_texture_set_by_name("death");

  set_mode(Mode::idle, true);

  return Ok();
}

Result<void, std::string> Player::update(const int64_t delta_time_ns) {
  duration_in_current_mode_ns_ += delta_time_ns;
  duration_since_last_exit_hit_ns_ += delta_time_ns;

  if (mode_ != Mode::dying && mode_ != Mode::dead) {
    position += (y_direction_ + x_direction_).cast<float>().normalized() *
                (static_cast<double>(delta_time_ns) / 1e9);
  }
  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }
  return Ok();
}

Result<void, std::string> Player::late_update() {
  update_mode();
  was_hit_ = false;
  return Ok();
}

void Player::update_mode() {
  if (mode_ == Mode::dead) {
    return;
  }

  if (mode_ == Mode::dying) {
    if (duration_in_current_mode_ns_ > max_duration_in_dying_ns_) {
      set_mode(Mode::dead);
    }
    return;
  }

  if (mode_ == Mode::being_hit) {
    if (duration_in_current_mode_ns_ > max_duration_in_being_hit_ns_) {
      duration_since_last_exit_hit_ns_ = 0L;
      set_mode(Mode::idle);
    }
    return;
  }

  if (was_hit_ && mode_ != Mode::being_hit &&
      duration_since_last_exit_hit_ns_ > cool_down_after_hit_ns_) {
    hp -= 1;
    if (hp <= 0) {
      set_mode(Mode::dying);
      was_hit_ = false;
    } else {
      set_mode(Mode::being_hit);
    }
    return;
  }

  if (attacking_) {
    if (attacking_dir_.x() > position.x()) {
      set_mode(Mode::attacking_right);
    } else {
      set_mode(Mode::attacking_left);
    }
    return;
  }

  if (x_direction_.norm() > 0 || y_direction_.norm() > 0) {
    if (x_direction_.x() > 0) {
      set_mode(Mode::walking_right);
      return;
    } else {
      set_mode(Mode::walking_left);
      return;
    }
  } else {
    set_mode(Mode::idle);
    return;
  }
}

void Player::set_mode(const Mode mode, const bool init) {
  if (mode == mode_ && !init) {
    return;
  }
  duration_in_current_mode_ns_ = 0L;

  remove_components<component::Animation>();
  mode_ = mode;
  std::vector<view::Texture> textures;
  float x_translation = 0.f;
  float fps = 15.;
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
  case Mode::attacking_left: {
    textures = attack_left_textures_;
    x_translation = -0.5f;
    break;
  }
  case Mode::attacking_right: {
    textures = attack_right_textures_;
    x_translation = 0.5f;
    break;
  }
  case Mode::being_hit: {
    textures = hit_textures_;
    fps = 20.f;
    break;
  }
  case Mode::dying:
    [[fallthrough]];
  case Mode::dead: {
    textures = dead_textures_;
    break;
  }
  }

  add_component<component::Animation>(
      [this, x_translation]() {
        return get_transform()
            .scale(Eigen::Vector2f{1.0, 1.3f})
            .translate(Eigen::Vector2f{x_translation, 0.17f});
      },
      std::move(textures), fps);
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
  } else if (key_press.key_event.code == sf::Keyboard::Escape) {
    attacking_ = true;
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

Result<bool, std::string>
Player::on_mouse_up(const view::MouseUpEvent &mouse_up) {
  if (mouse_up.button == view::MouseButton::Left) {
    attacking_ = false;
    attacking_dir_ = mouse_up.position;
    update_mode();
  }
  return Ok(true);
}

Result<bool, std::string>
Player::on_mouse_down(const view::MouseDownEvent &mouse_down) {
  if (mouse_down.button == view::MouseButton::Left) {
    attacking_ = true;
    attacking_dir_ = mouse_down.position;
    update_mode();
  }
  return Ok(true);
}

Result<bool, std::string>
Player::on_mouse_moved(const view::MouseMovedEvent &mouse_moved) {
  attacking_dir_ = mouse_moved.position;
  update_mode();
  return Ok(true);
}

Eigen::Affine2f Player::get_transform() const {
  float x_scale_factor;
  switch (mode_) {
  case Mode::idle: {
    x_scale_factor = 0.07;
    break;
  }

  case Mode::attacking_left:
    [[fallthrough]];
  case Mode::attacking_right: {
    x_scale_factor = 0.22;
    break;
  }
  case Mode::walking_left:
    [[fallthrough]];
  case Mode::walking_right: {
    x_scale_factor = 0.1;
    break;
  }

  case Mode::dying:
    [[fallthrough]];
  case Mode::dead:
    [[fallthrough]];
  case Mode::being_hit: {
    x_scale_factor = 0.1;
    break;
  }
  }

  return geometry::make_rectangle_from_center_and_size(
      position, Eigen::Vector2f{x_scale_factor, 0.1f});
}

Eigen::Affine2f Player::get_hurt_box_transform() const {
  switch (mode_) {
  case Mode::attacking_left: {
    return get_transform()
        .translate(Eigen::Vector2f{0.2f, 0.f})
        .scale(Eigen::Vector2f{0.2f, 1.f});
  }
  case Mode::attacking_right: {
    return get_transform()
        .translate(Eigen::Vector2f{-0.2f, 0.f})
        .scale(Eigen::Vector2f{0.2f, 1.f});
  }

  case Mode::walking_left: {
    return get_transform().scale(Eigen::Vector2f{0.7f, 1.f});
  }
  case Mode::walking_right: {
    return get_transform().scale(Eigen::Vector2f{0.7f, 1.f});
  }

  default: {
    return get_transform();
  }
  }
}

Eigen::Affine2f Player::get_hit_box_transform() const {
  switch (mode_) {
  case Mode::attacking_left: {
    return get_transform()
        .translate(Eigen::Vector2f{-0.5f, 0.f})
        .scale(Eigen::Vector2f{0.5f, 1.f});
  }
  case Mode::attacking_right: {
    return get_transform()
        .translate(Eigen::Vector2f{0.5f, 0.f})
        .scale(Eigen::Vector2f{0.5f, 1.f});
  }

  default: {
    return get_transform().scale(Eigen::Vector2f{0.0f, .0f});
  }
  }
}
} // namespace wiz
