#include "lightmaze/player.hh"
#include "components/animation.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/draw_rectangle.hh"
#include "components/gravity.hh"
#include "components/jumper.hh"
#include "components/light_emitter.hh"
#include "components/sprite.hh"
#include "lightmaze/components/lightmaze_light_volume.hh"
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
        // if we hit our head on something while jumping bounce down
        if (std::abs(translation.y()) > 0 && this->velocity_.y())
          this->velocity_.y() -= 0.5;
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

  // Add jumper component for jump mechanics
  jumper_component_ = add_component<component::Jumper>(
      [this]() {
        Eigen::Affine2f transform = get_transform();
        transform.translate(Eigen::Vector2f{0.f, -1.f});
        transform.scale(Eigen::Vector2f{1.f, .01f});

        return transform;
      },
      1 // max jumps (double jump)
  );

  // Add light emitter component for lighting gameplay
  light_emitter_component_ = add_component<component::LightEmitter>(
      component::LightEmitter::CircularLightParams{
          .transform_func = [this]() { return get_transform(); },
          .radius_meters = .8,
          .color = player_light_color_, // Pure white light
          .intensity = 1.0f             // Full intensity
      });

  // Add LightMazeLightVolume for color-based collision mechanics (50% larger
  // than player)
  light_volume_component_ = add_component<component::LightMazeLightVolume>(
      [this]() {
        Eigen::Affine2f transform = get_transform();
        transform.scale(1.5f); // Expand by 50%
        return transform;
      },
      player_light_color_ // same color as light emitter
  );

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
      if (jumper_component_->try_jump(Eigen::Vector2f{0.0, jump_speed_}).y() >
          0.f) {
        velocity_.y() = jump_speed_;
      }
    }
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
    return Ok(false); // Event handled, stop processing

  case sf::Keyboard::Right:
  case sf::Keyboard::D:
    move_right_pressed_ = true;
    return Ok(false); // Event handled, stop processing

  case sf::Keyboard::Up:
  case sf::Keyboard::W:
  case sf::Keyboard::Space:
    jump_pressed_ = true;
    return Ok(false); // Event handled, stop processing

  // Color selection keys
  case sf::Keyboard::Num1:
    set_light_color({255, 255, 255}); // White
    return Ok(false);                 // Event handled, stop processing

  case sf::Keyboard::Num2:
    set_light_color({255, 0, 0}); // Red
    return Ok(false);             // Event handled, stop processing

  case sf::Keyboard::Num3:
    set_light_color({0, 0, 255}); // Blue
    return Ok(false);             // Event handled, stop processing

  case sf::Keyboard::Num4:
    set_light_color({0, 255, 0}); // Green
    return Ok(false);             // Event handled, stop processing

  default:
    return Ok(true); // Event not handled, continue processing
  }
}

Result<bool, std::string>
Player::on_key_release(const view::KeyReleasedEvent &key_release) {
  switch (key_release.key_event.code) {
  case sf::Keyboard::Left:
  case sf::Keyboard::A:
    move_left_pressed_ = false;
    return Ok(false); // Event handled, stop processing

  case sf::Keyboard::Right:
  case sf::Keyboard::D:
    move_right_pressed_ = false;
    return Ok(false); // Event handled, stop processing

  case sf::Keyboard::Up:
  case sf::Keyboard::W:
  case sf::Keyboard::Space:
    jump_pressed_ = false;
    return Ok(false); // Event handled, stop processing

  default:
    return Ok(true); // Event not handled, continue processing
  }
}

Eigen::Affine2f Player::get_transform() const {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(position_);
  transform.scale(size_);
  return transform;
}

void Player::set_light_color(const view::Color &new_color) {
  player_light_color_ = new_color;

  // Update the light emitter component color if it exists
  if (light_emitter_component_) {
    light_emitter_component_->set_color(player_light_color_);
  }

  // Update the light volume component color if it exists
  if (light_volume_component_) {
    light_volume_component_->set_color(player_light_color_);
  }
}

} // namespace lightmaze
