#include "lightmaze/components/lightmaze_collider.hh"

namespace component {

LightMazeCollider::LightMazeCollider(GetTransformFunc get_transform,
                                     const view::Color& platform_color,
                                     CollisionCallback collision_callback)
    : NonCollidableAABBCollider(get_transform, collision_callback),
      platform_color_(platform_color) {
  // Set specific interaction type for LightMaze platform colliders
  set_interaction_type(InteractionType::lightmaze_platform_collider);
}

} // namespace component