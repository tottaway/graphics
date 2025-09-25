#include "components/light_emitter.hh"
#include <algorithm>
#include <cmath>

namespace component {

// CircularLightGeometry implementation

CircularLightGeometry::CircularLightGeometry(float radius_meters)
    : radius_meters_(radius_meters) {
  // Ensure positive radius
  if (radius_meters_ <= 0.0f) {
    radius_meters_ = 0.1f; // Fallback minimum
  }
}


void CircularLightGeometry::set_radius(float new_radius_meters) {
  if (new_radius_meters > 0.0f) {
    radius_meters_ = new_radius_meters;
  }
}

// LightEmitter implementation

LightEmitter::LightEmitter(const CircularLightParams& params)
    : transform_func_(params.transform_func),
      geometry_(std::make_shared<CircularLightGeometry>(params.radius_meters)),
      color_(params.color),
      intensity_(std::clamp(params.intensity, 0.0f, 1.0f)) {
}

LightEmitter::LightEmitter(const GlobalLightParams& params)
    : transform_func_([]() { return Eigen::Affine2f::Identity(); }), // Global lights don't need position
      geometry_(std::make_shared<GlobalLightGeometry>()),
      color_(params.color),
      intensity_(std::clamp(params.intensity, 0.0f, 1.0f)) {
}

LightEmitter::LightEmitter(const CustomGeometryLightParams& params)
    : transform_func_(params.transform_func),
      geometry_(params.geometry),
      color_(params.color),
      intensity_(std::clamp(params.intensity, 0.0f, 1.0f)) {

  // Ensure we have valid geometry
  if (!geometry_) {
    // Fallback to a small circular light
    geometry_ = std::make_shared<CircularLightGeometry>(1.0f);
  }
}

LightEmitter::LightInfo LightEmitter::get_light_info() const {
  // Get current world position from transform
  const Eigen::Affine2f transform = transform_func_();
  const Eigen::Vector2f world_position = transform.translation();

  return LightInfo{
    .world_position = world_position,
    .geometry = geometry_,
    .color = color_,
    .intensity = intensity_
  };
}

void LightEmitter::set_intensity(float new_intensity) {
  intensity_ = std::clamp(new_intensity, 0.0f, 1.0f);
}

void LightEmitter::set_color(const view::Color& new_color) {
  color_ = new_color;
}

void LightEmitter::set_geometry(std::shared_ptr<LightGeometry> new_geometry) {
  if (new_geometry) {
    geometry_ = new_geometry;
  }
}

} // namespace component