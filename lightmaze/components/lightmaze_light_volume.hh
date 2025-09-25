#pragma once
#include "components/collider.hh"
#include "view/screen.hh"
#include <Eigen/Dense>

namespace component {

/**
 * @brief Light volume component for LightMaze color-based collision detection
 *
 * This component represents a light emission area that can detect collisions
 * with LightMazeCollider components. It provides only collision detection - it
 * does NOT manage or modify other collider components. The LightMazeCollider
 * component is responsible for managing its own SolidAABBCollider based on
 * detected light volumes.
 *
 * The component extends NonCollidableAABBCollider to provide collision
 * detection without physical interaction (no position changes).
 */
class LightMazeLightVolume : public NonCollidableAABBCollider {
public:
  /**
   * @brief Construct a LightMazeLightVolume component
   * @param get_transform Function to get the entity's transform for collision
   * bounds
   * @param light_color RGB color of the light volume for matching
   * @param collision_callback Optional callback when collision is detected
   */
  LightMazeLightVolume(
      GetTransformFunc get_transform, const view::Color &light_color,
      CollisionCallback collision_callback = [](const model::EntityID) {});

  /**
   * @brief Get the light color for matching with platforms
   * @return RGB color of this light volume
   */
  [[nodiscard]] const view::Color &get_light_color() const {
    return light_color_;
  }

  /**
   * @brief Set the light color for matching with platforms
   * @param new_color New RGB color for this light volume
   * @post Light color updated to new_color
   */
  void set_color(const view::Color &new_color) {
    light_color_ = new_color;
  }

private:
  /// RGB color of the light volume for matching
  view::Color light_color_;
};

} // namespace component
