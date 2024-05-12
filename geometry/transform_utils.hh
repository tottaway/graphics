#pragma once

#include <Eigen/Dense>

namespace geometry {
[[nodiscard]] Eigen::Affine2f
transform_from_translation_and_scale(const Eigen::Vector2f &translation,
                                     const float scale);

[[nodiscard]] Eigen::Affine2f
transform_from_translation_and_scale(const Eigen::Vector2f &translation,
                                     const Eigen::Vector2f &scale);

[[nodiscard]] Eigen::Affine2f
transform_from_grid_cell(const Eigen::Vector2i grid_cell,
                         const Eigen::Vector2f grid_cell_size);
} // namespace geometry
