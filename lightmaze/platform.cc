#include "lightmaze/platform.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/draw_rectangle.hh"
#include "components/jump_reset.hh"
#include "model/game_state.hh"
#include "view/screen.hh"

namespace lightmaze {

Platform::Platform(model::GameState &game_state,
                   const Eigen::Vector2f &top_center_position,
                   const Eigen::Vector2f &size)
    : model::Entity(game_state), size_(size) {
  // Calculate the actual center position from the top center position
  // Move down by half the height to get the center
  position_ = top_center_position - Eigen::Vector2f{0.0f, size_.y() * 0.5f};
}

Result<void, std::string> Platform::init() {
  // Add white rectangle drawing component for visual representation
  add_component<component::DrawRectangle>([this]() {
    return component::DrawRectangle::RectangleInfo{
        .transform = get_transform(),
        .color = view::Color{255, 255, 255} // White color
    };
  });

  // Add solid collision component so player can stand on platform
  add_component<component::StaticAABBCollider>(
      [this]() { return get_transform(); });

  // Add jump reset component so players can jump again when landing on top of
  // platform
  add_component<component::JumpReset>([this]() {
    Eigen::Affine2f transform = get_transform();
    // Position the jump reset area only on the top surface of the platform
    // Move it up by half the platform height so it sits on top
    transform.translate(Eigen::Vector2f{0.0f, 1.0f});
    // Make it very thin (only 1% of platform height) and slightly wider for
    // better detection
    transform.scale(Eigen::Vector2f{.95f, 0.03f});
    return transform;
  });

  return Ok();
}

Result<void, std::string> Platform::update(const int64_t delta_time_ns) {
  // Platform is static, but need to update components for collision detection
  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }

  return Ok();
}

Eigen::Affine2f Platform::get_transform() const {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(position_);
  transform.scale(size_);
  return transform;
}

Eigen::Vector2f Platform::get_top_center_position() const {
  // Convert from center position back to top center position
  return position_ + Eigen::Vector2f{0.0f, size_.y() * 0.5f};
}

} // namespace lightmaze
