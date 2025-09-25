#pragma once
#include "components/collider.hh"
#include "view/screen.hh"
#include <Eigen/Dense>

namespace component {

/**
 * @brief Platform collider component for LightMaze color-based collision
 * detection
 *
 * This component detects collisions with LightMazeLightVolume components and
 * tracks which light colors are currently affecting it. It does NOT manage
 * other collider components - that responsibility lies with external systems or
 * the entity itself.
 *
 * The component extends NonCollidableAABBCollider to provide collision
 * detection without physical interaction (no position changes).
 */
class LightMazeCollider : public NonCollidableAABBCollider {
public:
  /**
   * @brief Construct a LightMazeCollider component
   * @param get_transform Function to get the entity's transform for collision
   * bounds
   * @param platform_color RGB color of the platform for matching with light
   * volumes
   * @param collision_callback Optional callback when collision is detected
   */
  LightMazeCollider(GetTransformFunc get_transform,
                    const view::Color &platform_color,
                    CollisionCallback collision_callback);

  /**
   * @brief Get the platform color for matching with light volumes
   * @return RGB color of this platform
   */
  [[nodiscard]] const view::Color &get_platform_color() const {
    return platform_color_;
  }

private:
  /// RGB color of the platform for matching
  view::Color platform_color_;
};

} // namespace component
