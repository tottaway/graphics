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

Eigen::Affine2f
transform_from_grid_cell(const Eigen::Vector2i grid_cell,
                         const Eigen::Vector2f grid_cell_size_length) {
  // divide by two because unit rectangle has a max side length of 2
  return Eigen::Affine2f(
             Eigen::Translation2f{
                 grid_cell.cast<float>().cwiseProduct(grid_cell_size_length)})
      .scale(grid_cell_size_length / 2);
}
} // namespace geometry
