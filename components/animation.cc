#include "components/animation.hh"

namespace component {
Animation::Animation(GetTransformFunc get_transform,
                     std::vector<view::Texture> textures, const float fps)
    : Sprite([this]() { return get_sprite_info(); }),
      get_transform_(std::move(get_transform)), textures_(textures),
      nanos_between_changes_(static_cast<int64_t>(1 / fps * 1e9)) {}

Result<void, std::string> Animation::update(const int64_t delta_time_ns) {
  nanos_since_last_change_ += delta_time_ns;
  if (nanos_since_last_change_ > nanos_between_changes_) {
    current_texture_index_ = (current_texture_index_ + 1) % textures_.size();
    nanos_since_last_change_ = 0L;
  }
  return Ok();
}

Animation::SpriteInfo Animation::get_sprite_info() {
  return SpriteInfo{get_transform_(), textures_[current_texture_index_]};
}
} // namespace component
