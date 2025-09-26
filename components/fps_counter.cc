#include "components/fps_counter.hh"
#include "geometry/rectangle_utils.hh"
#include <algorithm>
#include <cmath>

namespace component {

FpsCounter::FpsCounter(const FpsCounterParams &params) : params_(params) {
  // Initialize frame times array to prevent garbage values
  frame_times_ns_.fill(16'666'667L); // ~60 FPS initial estimate
}

Result<void, std::string> FpsCounter::update(const int64_t delta_time_ns) {
  // Record current frame time
  frame_times_ns_[frame_index_] = delta_time_ns;
  frame_index_ = (frame_index_ + 1) % frame_history_size_;

  // Update display text periodically to avoid flickering
  time_since_last_display_update_ns_ += delta_time_ns;
  if (time_since_last_display_update_ns_ >= update_interval_ns_) {
    update_display_text();
    time_since_last_display_update_ns_ = 0L;
  }

  return Ok();
}

Result<void, std::string> FpsCounter::draw(view::Screen &screen) const {
  // Get the current transform from the parent entity
  const auto transform = params_.transform_func();
  const auto [bottom_left, top_right] =
      geometry::get_bottom_left_and_top_right_from_transform(transform);

  // Draw FPS text
  const Eigen::Vector2f text_position =
      bottom_left +
      Eigen::Vector2f{0.01f, (top_right.y() - bottom_left.y()) * 0.5f};

  screen.draw_text(text_position, params_.font_size, current_fps_text_,
                   params_.text_color);

  return Ok();
}

uint32_t FpsCounter::calculate_average_fps_1s() const {
  // Calculate total time for frames within the last 1 second
  int64_t total_time_ns = 0;
  size_t valid_frames = 0;

  for (size_t i = 0; i < frame_history_size_; ++i) {
    if (frame_times_ns_[i] > 0 && frame_times_ns_[i] <= one_second_window_ns_) {
      total_time_ns += frame_times_ns_[i];
      valid_frames++;
    }
  }

  if (valid_frames == 0 || total_time_ns == 0) {
    return 0;
  }

  // Calculate average frame time and convert to FPS
  const int64_t average_frame_time_ns = total_time_ns / valid_frames;
  const double fps = 1e9 / static_cast<double>(average_frame_time_ns);

  return static_cast<uint32_t>(std::round(fps));
}

uint32_t FpsCounter::calculate_minimum_fps_1s() const {
  // Find the maximum frame time (which gives minimum FPS) within the last 1
  // second
  int64_t max_frame_time_ns = 0;

  for (size_t i = 0; i < frame_history_size_; ++i) {
    if (frame_times_ns_[i] > 0 && frame_times_ns_[i] <= one_second_window_ns_) {
      max_frame_time_ns = std::max(max_frame_time_ns, frame_times_ns_[i]);
    }
  }

  if (max_frame_time_ns == 0) {
    return 0;
  }

  // Convert maximum frame time to minimum FPS
  const double min_fps = 1e9 / static_cast<double>(max_frame_time_ns);
  return static_cast<uint32_t>(std::round(min_fps));
}

void FpsCounter::update_display_text() const {
  const uint32_t avg_fps = calculate_average_fps_1s();
  const uint32_t min_fps = calculate_minimum_fps_1s();
  current_fps_text_ =
      "FPS: " + std::to_string(avg_fps) + " Min: " + std::to_string(min_fps);
}

} // namespace component
