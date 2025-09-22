#include "components/jumper.hh"
#include "components/collider.hh"

namespace component {

Jumper::Jumper(GetTransformFunc get_transform, uint32_t max_jumps_allowed)
    : NonCollidableAABBCollider(get_transform,
                                [this](const model::EntityID other_entity_id) {
                                  handle_jump_reset_collision(other_entity_id);
                                }),
      max_jumps_allowed_(max_jumps_allowed) {
  set_interaction_type(InteractionType::jumper_collider);
}

Eigen::Vector2f Jumper::try_jump(const Eigen::Vector2f &desired_jump_velocity) {
  if (!can_jump()) {
    return Eigen::Vector2f{0.0f, 0.0f}; // No jump allowed, return zero velocity
  }

  jump_count_++;
  return desired_jump_velocity; // Jump allowed, return the desired velocity
}

uint32_t Jumper::get_jump_count() const { return jump_count_; }

uint32_t Jumper::get_max_jumps() const { return max_jumps_allowed_; }

bool Jumper::can_jump() const { return jump_count_ < max_jumps_allowed_; }

bool Jumper::is_grounded() const { return is_colliding_with_jump_reset_; }

void Jumper::reset_jumps() { jump_count_ = 0; }

void Jumper::set_max_jumps(uint32_t max_jumps) {
  max_jumps_allowed_ = max_jumps;
}

void Jumper::handle_jump_reset_collision(
    const model::EntityID other_entity_id) {
  // Mark that we are currently colliding with a jump reset
  is_colliding_with_jump_reset_ = true;
  // Reset jump count when colliding with a JumpReset collider
  reset_jumps();
}

Result<void, std::string> Jumper::update(const int64_t delta_time_ns) {
  // Check if we went from colliding to not colliding without jumping
  if (was_colliding_with_jump_reset_ && !is_colliding_with_jump_reset_) {
    // We left the ground without jumping, lose one jump to prevent coyote time
    if (jump_count_ == 0) {
      jump_count_++;
    }
  }

  // Update collision state for next frame
  was_colliding_with_jump_reset_ = is_colliding_with_jump_reset_;
  is_colliding_with_jump_reset_ =
      false; // Reset for next frame's collision detection

  return NonCollidableAABBCollider::update(delta_time_ns);
}

} // namespace component
