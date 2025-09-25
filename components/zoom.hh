#pragma once
#include "components/component.hh"
#include "view/screen.hh"
#include <Eigen/Geometry>
#include <functional>
#include <optional>

namespace component {

/**
 * @brief Component that manages viewport zoom level and updates screen viewport size
 *
 * The Zoom component allows entities to control the viewport zoom level by
 * modifying the viewport size during its draw() call. Smaller viewport sizes
 * result in zoomed-in views, while larger viewport sizes result in zoomed-out views.
 */
class Zoom : public Component {
public:
  static constexpr std::string_view component_type_name = "zoom_component";

  /**
   * @brief Construct a Zoom component
   * @param initial_zoom_level Initial zoom level (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out)
   * @pre initial_zoom_level > 0.0f
   * @post Base viewport size will be determined from screen on first draw() call
   */
  explicit Zoom(float initial_zoom_level = 1.0f);

  /**
   * @brief Get the component type name for identification
   * @return Static string identifying this component type
   */
  [[nodiscard]] virtual std::string_view get_component_type_name() const override {
    return component_type_name;
  }

  /**
   * @brief Update the viewport size based on current zoom level
   * @param screen Screen to update viewport size on
   * @return Ok() on success, Err(message) if update fails
   * @post Screen viewport size updated to reflect current zoom level
   */
  [[nodiscard]] Result<void, std::string>
  draw(view::Screen& screen) const override;

  /**
   * @brief Set the zoom level
   * @param new_zoom_level New zoom level (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out)
   * @pre new_zoom_level > 0.0f
   * @post Zoom level updated, viewport will be updated on next draw call
   */
  void set_zoom_level(float new_zoom_level);

  /**
   * @brief Get the current zoom level
   * @return Current zoom level
   */
  [[nodiscard]] float get_zoom_level() const { return zoom_level_; }

  /**
   * @brief Modify zoom level by a factor (for scroll wheel handling)
   * @param zoom_factor Factor to multiply zoom level by (>1.0 = zoom in, <1.0 = zoom out)
   * @pre zoom_factor > 0.0f
   * @post Zoom level updated, clamped to reasonable bounds
   */
  void apply_zoom_factor(float zoom_factor);

  /**
   * @brief Reset zoom to default level (1.0)
   * @post Zoom level set to 1.0, viewport will be updated on next draw call
   */
  void reset_zoom();

private:
  /// Current zoom level (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out)
  float zoom_level_;

  /// Base viewport size when zoom level is 1.0 (initialized on first draw call)
  mutable std::optional<Eigen::Vector2f> maybe_base_viewport_size_;

  /// Minimum allowed zoom level (maximum zoom out)
  static constexpr float min_zoom_level_{0.1f};

  /// Maximum allowed zoom level (maximum zoom in)
  static constexpr float max_zoom_level_{10.0f};
};

} // namespace component