#include "wiz/ui/fps_counter.hh"
#include "components/draw_rectangle.hh"
#include "components/label.hh"
#include "geometry/rectangle_utils.hh"
#include "view/screen.hh"
#include "wiz/player.hh"
#include <algorithm>

namespace wiz {

FpsCounter::FpsCounter(model::GameState &game_state)
    : model::Entity(game_state) {
  // Initialize frame times array to prevent garbage values
  frame_times_ns_.fill(16'666'667L); // ~60 FPS initial estimate
}

Result<void, std::string> FpsCounter::init() {
  // Add background rectangle for better text readability
  add_component<component::DrawRectangle>([this]() {
    return component::DrawRectangle::RectangleInfo{
        get_transform(), view::Color{0, 0, 0} // Black background
    };
  });

  // Add text label for FPS display
  add_component<component::Label>([this]() {
    // Position label at left edge of rectangle
    const auto rect_transform = get_transform();
    const Eigen::Vector2f rect_center = rect_transform.translation();
    const Eigen::Vector2f label_position =
        rect_center -
        Eigen::Vector2f{background_width_, -background_height_ / 2.f};

    auto label_transform = rect_transform;
    label_transform.translation() = label_position;

    return component::Label::TextInfo{current_fps_text_,
                                      view::Color{255, 255, 255}, // White text
                                      font_size_, label_transform};
  });

  return Ok();
}

Result<void, std::string> FpsCounter::update(int64_t delta_time_ns) {
  // Record current frame time
  frame_times_ns_[frame_index_] = delta_time_ns;
  frame_index_ = (frame_index_ + 1) % frame_history_size_;

  // Update display text periodically to avoid flickering
  time_since_last_display_update_ns_ += delta_time_ns;
  if (time_since_last_display_update_ns_ >= update_interval_ns_) {
    update_display_text();
    time_since_last_display_update_ns_ = 0L;
  }

  // Update components
  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }

  return Ok();
}

Eigen::Affine2f FpsCounter::get_transform() const {
  // Position 0.2m to the right of the player
  Eigen::Vector2f position{-1.2f, 1.1f}; // Default fallback position

  auto maybe_player = game_state_.get_entity_pointer_by_type<Player>();
  if (maybe_player.isOk()) {
    const auto player = maybe_player.unwrap();
    position =
        player->position + Eigen::Vector2f{0.4f, 0.0f}; // 0.4m to the right
  }

  return geometry::make_rectangle_from_center_and_size(
      position, Eigen::Vector2f{background_width_, background_height_});
}

int32_t FpsCounter::calculate_average_fps_1s() const {
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

  return static_cast<int32_t>(std::round(fps));
}

int32_t FpsCounter::calculate_minimum_fps_1s() const {
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
  return static_cast<int32_t>(std::round(min_fps));
}

void FpsCounter::update_display_text() {
  const int32_t avg_fps = calculate_average_fps_1s();
  const int32_t min_fps = calculate_minimum_fps_1s();
  current_fps_text_ =
      "FPS: " + std::to_string(avg_fps) + " Min: " + std::to_string(min_fps);
}

} // namespace wiz
