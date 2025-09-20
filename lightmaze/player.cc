#include "lightmaze/player.hh"
#include "components/animation.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/gravity.hh"
#include "components/jumper.hh"
#include "components/sprite.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <filesystem>

namespace lightmaze {

Player::Player(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Player::init() {
  // Add centering component for automatic positioning
  add_component<component::Center>([this]() { return get_transform(); });

  // Add collision component for physics
  add_component<component::SolidAABBCollider>(
      [this]() { return get_transform(); },
      [this](const Eigen::Vector2f &translation) {
        this->position_ += translation;
      });

  // Load player textures and add sprite component
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(player_texture_set_path)));

  // For now, just use the idle animation
  auto idle_textures = texture_set->get_texture_set_by_name("idle");
  add_component<component::Animation>([this]() { return get_transform(); },
                                      idle_textures,
                                      5.0f // 5 fps animation
  );

  // Add gravity component for physics
  add_component<component::Gravity>(
      [this]() { return velocity_; },           // get velocity
      [this](const Eigen::Vector2f &velocity) { // set velocity
        velocity_ = velocity;
      },
      Eigen::Vector2f{0.0f, gravity_} // acceleration vector
  );

  // Add jumper component for jump mechanics (extend slightly for better
  // collision detection)
  auto *jumper = add_component<component::Jumper>(
      [this]() {
        Eigen::Affine2f transform = get_transform();
        // Extend the jumper collision area by 3% in all directions for more
        // reliable detection
        transform.scale(Eigen::Vector2f{1.03f, 1.03f});
        return transform;
      },
      1 // max jumps (double jump)
  );

  // Cache the Jumper component pointer for efficient access
  jumper_component_ = dynamic_cast<component::Jumper *>(jumper);

  return Ok();
}

Result<void, std::string> Player::update(const int64_t delta_time_ns) {
  const float delta_time_s =
      static_cast<float>(delta_time_ns) / 1'000'000'000.0f;

  // Update horizontal velocity based on input
  velocity_.x() = 0.0f;
  if (move_left_pressed_) {
    velocity_.x() -= move_speed_;
  }
  if (move_right_pressed_) {
    velocity_.x() += move_speed_;
  }

  // Reset Y velocity when grounded to prevent bouncing
  if (jumper_component_ != nullptr && jumper_component_->is_grounded()) {
    velocity_.y() = 0.0f; // Stop vertical movement when on ground
  }

  // Handle jumping using cached Jumper component (only once per key press)
  if (jump_pressed_) {
    if (jumper_component_ != nullptr) {
      Eigen::Vector2f jump_velocity =
          jumper_component_->try_jump(Eigen::Vector2f{0.0f, jump_speed_});
      velocity_.y() =
          jump_velocity.y(); // Add jump velocity (0 if no jump allowed)
    }
    jump_pressed_ = false; // Reset flag after attempting jump (must release and
                           // press again)
  }

  // Update position
  position_ += velocity_ * delta_time_s;

  // Ground detection is now handled by the Jumper component through JumpReset
  // collisions

  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }

  return Ok();
}

Result<bool, std::string>
Player::on_key_press(const view::KeyPressedEvent &key_press) {
  switch (key_press.key_event.code) {
  case sf::Keyboard::Left:
  case sf::Keyboard::A:
    move_left_pressed_ = true;
    return Ok(true);

  case sf::Keyboard::Right:
  case sf::Keyboard::D:
    move_right_pressed_ = true;
    return Ok(true);

  case sf::Keyboard::Up:
  case sf::Keyboard::W:
  case sf::Keyboard::Space:
    jump_pressed_ = true;
    return Ok(true);

  default:
    return Ok(false);
  }
}

Result<bool, std::string>
Player::on_key_release(const view::KeyReleasedEvent &key_release) {
  switch (key_release.key_event.code) {
  case sf::Keyboard::Left:
  case sf::Keyboard::A:
    move_left_pressed_ = false;
    return Ok(true);

  case sf::Keyboard::Right:
  case sf::Keyboard::D:
    move_right_pressed_ = false;
    return Ok(true);

  case sf::Keyboard::Up:
  case sf::Keyboard::W:
  case sf::Keyboard::Space:
    jump_pressed_ = false;
    return Ok(true);

  default:
    return Ok(false);
  }
}

Eigen::Affine2f Player::get_transform() const {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(position_);
  transform.scale(size_);
  return transform;
}

} // namespace lightmaze
