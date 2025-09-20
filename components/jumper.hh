#pragma once

#include "components/collider.hh"
#include "components/component.hh"
#include <cstdint>
#include <Eigen/Dense>

namespace component {

/**
 * @brief Jumper collider that tracks jump count and resets on ground contact
 *
 * Entities with this component can track how many times they've jumped
 * since last touching a JumpReset collider (ground/platform). This enables
 * features like double jumps, coyote time, and jump buffering.
 */
class Jumper : public NonCollidableAABBCollider {
public:
  static constexpr std::string_view collider_type_name = "jumper_collider";

  /**
   * @brief Get the collider type name for identification
   * @return Static string identifying this collider type
   */
  [[nodiscard]] virtual std::string_view get_collider_type_name() const {
    return collider_type_name;
  }

  /**
   * @brief Construct a new Jumper collider
   * @param get_transform Function to get the collider's transform
   * @param max_jumps_allowed Maximum jumps before requiring ground contact
   * @post Collider will track jumps and reset when touching JumpReset colliders
   */
  Jumper(GetTransformFunc get_transform, uint32_t max_jumps_allowed = 1);

  /**
   * @brief Attempt to perform a jump with given velocity
   * @param desired_jump_velocity Velocity to apply if jump is allowed
   * @return Jump velocity to add (desired velocity if allowed, zero vector if not)
   * @post Jump count incremented if jump was allowed
   */
  Eigen::Vector2f try_jump(const Eigen::Vector2f &desired_jump_velocity);

  /**
   * @brief Get current jump count since last ground contact
   * @return Number of jumps performed since last JumpReset collision
   */
  [[nodiscard]] uint32_t get_jump_count() const;

  /**
   * @brief Get maximum allowed jumps
   * @return Maximum jumps before requiring ground contact
   */
  [[nodiscard]] uint32_t get_max_jumps() const;

  /**
   * @brief Check if entity can currently jump
   * @return true if within jump limit, false otherwise
   */
  [[nodiscard]] bool can_jump() const;

  /**
   * @brief Check if currently colliding with a jump reset collider
   * @return true if colliding with ground/platform, false otherwise
   */
  [[nodiscard]] bool is_grounded() const;

  /**
   * @brief Reset jump count (called automatically on JumpReset collision)
   * @post Jump count set to 0, can_jump() returns true
   */
  void reset_jumps();

  /**
   * @brief Set maximum allowed jumps
   * @param max_jumps New maximum jump count
   * @post Future jump attempts will use the new limit
   */
  void set_max_jumps(uint32_t max_jumps);

  /**
   * @brief Update component state and handle coyote time prevention
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   * @post Tracks collision state and prevents coyote time jumps
   */
  [[nodiscard]] virtual Result<void, std::string> update(const int64_t delta_time_ns);

private:
  /// Current number of jumps since last ground contact
  uint32_t jump_count_{0};

  /// Maximum jumps allowed before requiring ground contact
  uint32_t max_jumps_allowed_;

  /// Whether we were colliding with a jump reset in the previous frame
  bool was_colliding_with_jump_reset_{false};

  /// Whether we are currently colliding with a jump reset this frame
  bool is_colliding_with_jump_reset_{false};

  /**
   * @brief Handle collision with JumpReset collider
   * @param other_entity_id ID of the entity we collided with
   */
  void handle_jump_reset_collision(const model::EntityID other_entity_id);
};

} // namespace component