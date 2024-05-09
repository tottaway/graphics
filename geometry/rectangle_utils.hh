#pragma once

#include <Eigen/Dense>

namespace geometry {
[[nodiscard]] std::pair<Eigen::Vector2f, Eigen::Vector2f>
get_bottom_left_and_top_right_from_transform(const Eigen::Affine2f &transform);

[[nodiscard]] bool rectangle_contains_point(const Eigen::Affine2f &transform,
                                            const Eigen::Vector2f &point);

[[nodiscard]] Eigen::Affine2f
make_square_from_center_and_size(const Eigen::Vector2f &center,
                                 const float side_length);

[[nodiscard]] Eigen::Affine2f
make_rectangle_from_center_and_size(const Eigen::Vector2f &center,
                                    const Eigen::Vector2f &side_lengths);
} // namespace geometry
