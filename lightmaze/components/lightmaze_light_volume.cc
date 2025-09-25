#include "lightmaze/components/lightmaze_light_volume.hh"

namespace component {

LightMazeLightVolume::LightMazeLightVolume(GetTransformFunc get_transform,
                                           const view::Color &light_color,
                                           CollisionCallback collision_callback)
    : NonCollidableAABBCollider(get_transform, collision_callback),
      light_color_(light_color) {
  // Set specific interaction type for LightMaze light volumes
  set_interaction_type(InteractionType::lightmaze_light_volume);
}

} // namespace component
