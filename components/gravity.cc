#include "components/gravity.hh"
#include <chrono>

namespace component {

Gravity::Gravity(GetVelocityFunc get_velocity_func, SetVelocityFunc set_velocity_func,
                 const Eigen::Vector2f &acceleration_m_per_s2)
    : get_velocity_func_(std::move(get_velocity_func)),
      set_velocity_func_(std::move(set_velocity_func)),
      acceleration_m_per_s2_(acceleration_m_per_s2) {}

Result<void, std::string> Gravity::update(const int64_t delta_time_ns) {
  if (!enabled_) {
    return Ok();
  }

  // Convert nanoseconds to seconds for physics calculations
  const float delta_time_s = static_cast<float>(delta_time_ns) / 1'000'000'000.0f;

  // Get current velocity from entity
  const Eigen::Vector2f current_velocity = get_velocity_func_();

  // Apply gravitational acceleration: v = v0 + a*t
  const Eigen::Vector2f new_velocity =
      current_velocity + acceleration_m_per_s2_ * delta_time_s;

  // Set the updated velocity back to the entity
  set_velocity_func_(new_velocity);

  return Ok();
}

void Gravity::set_acceleration(const Eigen::Vector2f &acceleration_m_per_s2) {
  acceleration_m_per_s2_ = acceleration_m_per_s2;
}

const Eigen::Vector2f &Gravity::get_acceleration() const {
  return acceleration_m_per_s2_;
}

void Gravity::set_enabled(bool enabled) {
  enabled_ = enabled;
}

bool Gravity::is_enabled() const {
  return enabled_;
}

} // namespace component