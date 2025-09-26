#pragma once
#include "components/component.hh"
#include "view/screen.hh"
#include "utility/try.hh"
#include <Eigen/Geometry>
#include <array>
#include <functional>
#include <string>

namespace component {

/**
 * @brief Component that displays FPS information with background rectangle
 *
 * This component provides real-time FPS monitoring by tracking frame delta times
 * and calculating rolling averages. It renders a background rectangle and text label
 * at a position provided by the parent entity, making it reusable across different games.
 *
 * The FPS counter shows both average and minimum FPS over the last 1 second window
 * to provide comprehensive performance information.
 */
class FpsCounter : public Component {
public:
  static constexpr std::string_view component_type_name = "fps_counter_component";

  /**
   * @brief Function type for providing the transform where FPS counter should be drawn
   */
  using TransformFunc = std::function<Eigen::Affine2f()>;

  /**
   * @brief Parameters for configuring the FPS counter appearance
   */
  struct FpsCounterParams {
    TransformFunc transform_func;           ///< Function to get position and size for FPS display
    float font_size{24.0f};                ///< Font size for the text
    view::Color text_color{255, 255, 255}; ///< Color for the FPS text (default: white)
    view::Color bg_color{0, 0, 0};         ///< Background rectangle color (default: black)
  };

  /**
   * @brief Construct FPS counter component
   * @param params Configuration parameters for the FPS counter
   * @post Component ready to track and display FPS information
   */
  explicit FpsCounter(const FpsCounterParams& params);

  /**
   * @brief Get the component type name for identification
   * @return Static string identifying this component type
   */
  [[nodiscard]] virtual std::string_view get_component_type_name() const override {
    return component_type_name;
  }

  /**
   * @brief Update FPS calculations and display text
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   * @post Frame time recorded and display text updated periodically
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(const int64_t delta_time_ns) override;

  /**
   * @brief Draw the FPS counter background and text
   * @param screen Screen to draw to
   * @return Ok() on success, Err(message) if drawing fails
   * @post Background rectangle and FPS text rendered at current transform
   */
  [[nodiscard]] virtual Result<void, std::string>
  draw(view::Screen& screen) const override;

private:
  /// Configuration parameters
  FpsCounterParams params_;

  /// Frame time tracking
  static constexpr size_t frame_history_size_{200}; // Track last 200 frames (~3.3s at 60fps)
  static constexpr int64_t update_interval_ns_{100'000'000L}; // Update display every 100ms
  static constexpr int64_t one_second_window_ns_{1'000'000'000L}; // 1 second window

  std::array<int64_t, frame_history_size_> frame_times_ns_{};
  size_t frame_index_{0};
  int64_t time_since_last_display_update_ns_{0L};
  mutable std::string current_fps_text_{"FPS: -- Min: --"};

  /**
   * @brief Calculate average FPS from frame history over last 1 second
   * @return Average FPS over the last 1 second
   */
  [[nodiscard]] uint32_t calculate_average_fps_1s() const;

  /**
   * @brief Calculate minimum FPS from frame history over last 1 second
   * @return Minimum FPS over the last 1 second (based on maximum frame gap)
   */
  [[nodiscard]] uint32_t calculate_minimum_fps_1s() const;

  /**
   * @brief Update the display text with current average and minimum FPS
   * @post current_fps_text_ updated with latest FPS calculations
   */
  void update_display_text() const;
};

} // namespace component