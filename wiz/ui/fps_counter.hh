#pragma once
#include "model/game_state.hh"
#include <array>
#include <cstdint>

namespace wiz {

/**
 * @brief UI entity that displays current FPS in the top-left corner.
 *
 * This entity tracks frame delta times and displays a rolling average FPS
 * to provide smooth, readable performance monitoring. It uses a background
 * rectangle for better text visibility and updates the display periodically
 * to avoid text flickering.
 */
class FpsCounter : public model::Entity {
public:
  static constexpr std::string_view entity_type_name = "wiz_fps_counter";

  /**
   * @brief Constructs an FPS counter entity.
   * @param game_state Reference to the game state
   */
  explicit FpsCounter(model::GameState &game_state);

  /**
   * @brief Initializes the FPS counter with background rectangle and text
   * label.
   * @return Ok() on success, Err(message) if component creation fails
   */
  Result<void, std::string> init();

  [[nodiscard]] virtual std::string_view get_entity_type_name() const override {
    return entity_type_name;
  }

  /**
   * @brief Updates FPS calculation and display text.
   * @param delta_time_ns Time elapsed since last update in nanoseconds
   * @return Ok() on success, Err(message) if update fails
   */
  [[nodiscard]] virtual Result<void, std::string>
  update(int64_t delta_time_ns) override;

  /**
   * @brief Returns transform for positioning (top-left corner).
   * @return Transform matrix for the FPS counter
   */
  [[nodiscard]] virtual Eigen::Affine2f get_transform() const override;

  [[nodiscard]] virtual uint8_t get_z_level() const override {
    return 4;
  } // Always on top

private:
  static constexpr size_t frame_history_size_{
      200}; // Track last 200 frames (1 second at 200fps)
  static constexpr int64_t update_interval_ns_{
      100'000'000L}; // Update display every 100ms
  static constexpr int64_t one_second_window_ns_{
      1'000'000'000L}; // 1 second window
  static constexpr float font_size_{32.0f};
  static constexpr float background_width_{0.2f};   // Wider for more text
  static constexpr float background_height_{0.05f}; // Single line height

  std::array<int64_t, frame_history_size_> frame_times_ns_{};
  size_t frame_index_{0};
  int64_t time_since_last_display_update_ns_{0L};
  std::string current_fps_text_{"FPS: -- Min: --"};

  /**
   * @brief Calculates average FPS from frame history over last 1 second.
   * @return Average FPS over the last 1 second
   */
  [[nodiscard]] int32_t calculate_average_fps_1s() const;

  /**
   * @brief Calculates minimum FPS from frame history over last 1 second.
   * @return Minimum FPS over the last 1 second (based on maximum frame gap)
   */
  [[nodiscard]] int32_t calculate_minimum_fps_1s() const;

  /**
   * @brief Updates the display text with current average and minimum FPS.
   */
  void update_display_text();
};

} // namespace wiz
