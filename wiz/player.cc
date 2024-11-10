#include "wiz/player.hh"
#include "components/animation.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/draw_rectangle.hh"
#include "components/hit_box.hh"
#include "components/hurt_box.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/components/character_animation_set.hh"
#include "wiz/components/health_bar.hh"
#include "wiz/components/hit_hurt_boxes.hh"

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

  add_component<HealthBar>(
      hp, [this]() { return hp; }, [this]() { return get_transform(); });

  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(player_texture_set_path)));

  add_component<CharacterAnimationSet>(
      [this]() { return get_animation_transform(); },
      [this]() { return mode_; },
      CharacterTextureSet{texture_set->get_texture_set_by_name("idle"),
                          texture_set->get_texture_set_by_name("move_right"),
                          texture_set->get_texture_set_by_name("move_left"),
                          texture_set->get_texture_set_by_name("attack_right"),
                          texture_set->get_texture_set_by_name("attack_left"),
                          texture_set->get_texture_set_by_name("take_hit"),
                          texture_set->get_texture_set_by_name("death"), 10.f,
                          10.f, 10.f, 10.f, 10.f, 15.f, 10.f});

  set_mode(CharacterMode::idle, true);

  return Ok();
}

Result<void, std::string> Player::update(const int64_t delta_time_ns) {
  duration_in_current_mode_ns_ += delta_time_ns;
  duration_since_last_exit_hit_ns_ += delta_time_ns;

  if (mode_ != CharacterMode::dying && mode_ != CharacterMode::dead) {
    position += (y_direction_ + x_direction_).cast<float>().normalized() *
                (static_cast<double>(delta_time_ns) / 1e9) * 0.5f;
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
  if (mode_ == CharacterMode::dead) {
    return;
  }

  if (mode_ == CharacterMode::dying) {
    if (duration_in_current_mode_ns_ > max_duration_in_dying_ns_) {
      set_mode(CharacterMode::dead);
    }
    return;
  }

  if (mode_ == CharacterMode::being_hit) {
    if (duration_in_current_mode_ns_ > max_duration_in_being_hit_ns_) {
      duration_since_last_exit_hit_ns_ = 0L;
      set_mode(CharacterMode::idle);
    }
    return;
  }

  if (was_hit_ && mode_ != CharacterMode::being_hit &&
      duration_since_last_exit_hit_ns_ > cool_down_after_hit_ns_) {
    hp -= 1;
    if (hp <= 0) {
      set_mode(CharacterMode::dying);
      was_hit_ = false;
    } else {
      set_mode(CharacterMode::being_hit);
    }
    return;
  }

  if (attacking_) {
    if (attacking_dir_.x() > position.x()) {
      set_mode(CharacterMode::attacking_right);
    } else {
      set_mode(CharacterMode::attacking_left);
    }
    return;
  }

  if (x_direction_.norm() > 0 || y_direction_.norm() > 0) {
    if (x_direction_.x() > 0) {
      set_mode(CharacterMode::walking_right);
      return;
    } else {
      set_mode(CharacterMode::walking_left);
      return;
    }
  } else {
    set_mode(CharacterMode::idle);
    return;
  }
}

void Player::set_mode(const CharacterMode mode, const bool init) {
  if (mode == mode_ && !init) {
    return;
  }
  duration_in_current_mode_ns_ = 0L;
  mode_ = mode;
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
  return geometry::make_rectangle_from_center_and_size(
      position, Eigen::Vector2f{.1f, .1f});
}

Eigen::Affine2f Player::get_hurt_box_transform() const {
  switch (mode_) {
  case CharacterMode::attacking_left: {
    return get_animation_transform()
        .translate(Eigen::Vector2f{0.7f, 0.f})
        .scale(Eigen::Vector2f{0.22f, 1.f});
  }
  case CharacterMode::attacking_right: {
    return get_animation_transform()
        .translate(Eigen::Vector2f{-0.7f, 0.f})
        .scale(Eigen::Vector2f{0.22f, 1.f});
  }

  case CharacterMode::walking_left: {
    return get_animation_transform().scale(Eigen::Vector2f{0.7f, 1.f});
  }
  case CharacterMode::walking_right: {
    return get_animation_transform().scale(Eigen::Vector2f{0.7f, 1.f});
  }

  default: {
    return get_animation_transform();
  }
  }
}

Eigen::Affine2f Player::get_hit_box_transform() const {
  switch (mode_) {
  case CharacterMode::attacking_left: {
    return get_animation_transform()
        .translate(Eigen::Vector2f{-0.3f, 0.f})
        .scale(Eigen::Vector2f{0.7f, 1.f});
  }
  case CharacterMode::attacking_right: {
    return get_animation_transform()
        .translate(Eigen::Vector2f{0.3f, 0.f})
        .scale(Eigen::Vector2f{0.7f, 1.f});
  }

  default: {
    return get_transform().scale(Eigen::Vector2f{0.0f, .0f});
  }
  }
}

Eigen::Affine2f Player::get_animation_transform() const {
  float x_scale_factor = 1.f;
  float y_scale_factor = 1.f;
  float x_translation = 0.f;
  float y_translation = 0.f;

  switch (mode_) {
  case CharacterMode::idle: {
    x_scale_factor = 0.7f;
    break;
  }

  case CharacterMode::attacking_left:
    x_translation = -.5f;
    x_scale_factor = 1.5f;
    y_scale_factor = 0.8f;
    break;
  case CharacterMode::attacking_right: {
    x_translation = .5f;
    x_scale_factor = 1.5f;
    y_scale_factor = 0.8f;
    break;
  }
  case CharacterMode::walking_left:
    [[fallthrough]];
  case CharacterMode::walking_right: {
    x_scale_factor = 1.f;
    break;
  }

  case CharacterMode::dying:
    [[fallthrough]];
  case CharacterMode::dead:
    [[fallthrough]];
  case CharacterMode::being_hit: {
    x_scale_factor = 1.f;
    break;
  }
  }
  return get_transform()
      .scale(Eigen::Vector2f{x_scale_factor, y_scale_factor})
      .translate(Eigen::Vector2f{x_translation, y_translation});
}
} // namespace wiz
