#include "components/zoom.hh"
#include <algorithm>
#include <cmath>

namespace component {

Zoom::Zoom(float initial_zoom_level)
    : zoom_level_(std::clamp(initial_zoom_level, min_zoom_level_, max_zoom_level_)) {
  // Base viewport size will be initialized on first draw() call
}

Result<void, std::string> Zoom::draw(view::Screen& screen) const {
  // Initialize base viewport size on first call
  if (!maybe_base_viewport_size_.has_value()) {
    maybe_base_viewport_size_ = screen.get_viewport_size();
  }

  // Calculate the effective viewport size based on zoom level
  // Higher zoom levels mean smaller viewport (more zoomed in)
  // Lower zoom levels mean larger viewport (more zoomed out)
  const Eigen::Vector2f effective_viewport_size = maybe_base_viewport_size_.value() / zoom_level_;

  // Update the screen's viewport size
  screen.set_viewport_size(effective_viewport_size);

  return Ok();
}

void Zoom::set_zoom_level(float new_zoom_level) {
  zoom_level_ = std::clamp(new_zoom_level, min_zoom_level_, max_zoom_level_);
}

void Zoom::apply_zoom_factor(float zoom_factor) {
  if (zoom_factor <= 0.0f) {
    return; // Invalid factor, ignore
  }

  const float new_zoom_level = zoom_level_ * zoom_factor;
  set_zoom_level(new_zoom_level);
}

void Zoom::reset_zoom() {
  zoom_level_ = 1.0f;
}

} // namespace component