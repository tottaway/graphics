#include "geometry/transform_utils.hh"

namespace geometry {

Eigen::Affine2f
transform_from_translation_and_scale(const Eigen::Vector2f &translation,
                                     const float scale) {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(translation);
  transform.scale(scale);
  return transform;
}

Eigen::Affine2f
transform_from_translation_and_scale(const Eigen::Vector2f &translation,
                                     const Eigen::Vector2f &scale) {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(translation);
  transform.scale(scale);
  return transform;
}
} // namespace geometry
