#include "geometry/rectangle_utils.hh"

namespace geometry {
std::pair<Eigen::Vector2f, Eigen::Vector2f>
get_bottom_left_and_top_right_from_transform(const Eigen::Affine2f &transform) {
  const Eigen::Vector2f bottom_left{-1.0f, -1.0f};
  const Eigen::Vector2f top_right{1.0f, 1.0f};
  return {transform * bottom_left, transform * top_right};
}

bool rectangle_contains_point(const Eigen::Affine2f &transform,
                              const Eigen::Vector2f &point) {
  const auto transformed_point = transform.inverse() * point;
  return transformed_point.x() < 1.0f && transformed_point.x() > -1.0f &&
         transformed_point.y() < 1.0f && transformed_point.x() > -1.0f;
}

Eigen::Affine2f make_square_from_center_and_size(const Eigen::Vector2f &center,
                                                 const float side_length) {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(center);
  transform.scale(side_length);
  return transform;
}

Eigen::Affine2f
make_rectangle_from_center_and_size(const Eigen::Vector2f &center,
                                    const Eigen::Vector2f &side_lengths) {
  Eigen::Affine2f transform = Eigen::Affine2f::Identity();
  transform.translate(center);
  transform.scale(side_lengths);
  return transform;
}
} // namespace geometry
