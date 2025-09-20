#pragma once

#include "components/collider.hh"
#include "components/component.hh"

namespace component {

/**
 * @brief JumpReset collider that resets jump count when touched
 *
 * Platforms and ground surfaces should have this collider to reset
 * the player's jump count, allowing them to jump again when they
 * land on a surface.
 */
class JumpReset : public NonCollidableAABBCollider {
public:
  static constexpr std::string_view collider_type_name = "jump_reset_collider";

  [[nodiscard]] virtual std::string_view get_collider_type_name() const override {
    return collider_type_name;
  }

  /**
   * @brief Construct a new JumpReset collider
   * @param get_transform Function to get the collider's transform
   * @post Collider will reset jump count on entities that touch it
   */
  JumpReset(GetTransformFunc get_transform);
};

} // namespace component