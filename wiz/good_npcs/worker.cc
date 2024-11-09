#include "wiz/good_npcs/worker.hh"
#include "geometry/rectangle_utils.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/components/character_animation_set.hh"
#include "wiz/map/grass_tile.hh"
#include "wiz/map/map.hh"
#include <random>

namespace wiz {
Worker::Worker(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Worker::init(const Eigen::Vector2f position) {

  position_ = position;
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(worker_texture_set_path)));

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 5);

  CharacterTextureSet color_texture_set{
      texture_set->get_texture_set_by_name("cyan_idle"),
      texture_set->get_texture_set_by_name("cyan_walk_right"),
      texture_set->get_texture_set_by_name("cyan_walk_left"),
      texture_set->get_texture_set_by_name("cyan_attack_right"),
      texture_set->get_texture_set_by_name("cyan_attack_left"),
      texture_set->get_texture_set_by_name("cyan_idle"),
      texture_set->get_texture_set_by_name("cyan_idle"),
      10.f,
      10.f,
      10.f,
      10.f,
      10.f,
      10.f,
      10.f};
  if (dist(rng) == 1) {
    // pass in order to stick with cyan
  } else if (dist(rng) == 2) {
    color_texture_set.idle_textures =
        texture_set->get_texture_set_by_name("purple_idle");
    color_texture_set.walk_right_textures =
        texture_set->get_texture_set_by_name("purple_walk_right");
    color_texture_set.walk_left_textures =
        texture_set->get_texture_set_by_name("purple_walk_left");
    color_texture_set.attack_right_textures =
        texture_set->get_texture_set_by_name("purple_attack_right");
    color_texture_set.attack_left_textures =
        texture_set->get_texture_set_by_name("purple_attack_left");
    color_texture_set.hit_textures =
        texture_set->get_texture_set_by_name("purple_idle");
    color_texture_set.dead_textures =
        texture_set->get_texture_set_by_name("purple_idle");
  } else if (dist(rng) == 3) {
    color_texture_set.idle_textures =
        texture_set->get_texture_set_by_name("lime_idle");
    color_texture_set.walk_right_textures =
        texture_set->get_texture_set_by_name("lime_walk_right");
    color_texture_set.walk_left_textures =
        texture_set->get_texture_set_by_name("lime_walk_left");
    color_texture_set.attack_right_textures =
        texture_set->get_texture_set_by_name("lime_attack_right");
    color_texture_set.attack_left_textures =
        texture_set->get_texture_set_by_name("lime_attack_left");
    color_texture_set.hit_textures =
        texture_set->get_texture_set_by_name("lime_idle");
    color_texture_set.dead_textures =
        texture_set->get_texture_set_by_name("lime_idle");
  } else if (dist(rng) == 4) {
    color_texture_set.idle_textures =
        texture_set->get_texture_set_by_name("red_idle");
    color_texture_set.walk_right_textures =
        texture_set->get_texture_set_by_name("red_walk_right");
    color_texture_set.walk_left_textures =
        texture_set->get_texture_set_by_name("red_walk_left");
    color_texture_set.attack_right_textures =
        texture_set->get_texture_set_by_name("red_attack_right");
    color_texture_set.attack_left_textures =
        texture_set->get_texture_set_by_name("red_attack_left");
    color_texture_set.hit_textures =
        texture_set->get_texture_set_by_name("red_idle");
    color_texture_set.dead_textures =
        texture_set->get_texture_set_by_name("red_idle");
  }
  add_component<CharacterAnimationSet>([this]() { return get_transform(); },
                                       [this]() { return mode_; },
                                       color_texture_set);

  return Ok();
}

Result<void, std::string> Worker::update_direction() {
  const auto map = TRY(game_state_.get_entity_pointer_by_type<Map>());
  const auto current_tile_id =
      TRY(map->get_map_tile_entity_by_position(position_));
  const auto maybe_grass_tile =
      game_state_.get_entity_pointer_by_id_as<GrassTile>(current_tile_id);

  if (maybe_grass_tile.isErr()) {
    off_flowers_ = true;
    return Ok();
  }
  const auto grass_tile = maybe_grass_tile.unwrap();
  off_flowers_ = !grass_tile->has_flowers();
  return Ok();
}

Result<void, std::string> Worker::update(const int64_t delta_time_ns) {
  update_direction();
  position_ += direction_ * (static_cast<double>(delta_time_ns) / 1e9);

  mode_ = CharacterMode::walking_right;

  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }
  return Ok();
}

Eigen::Affine2f Worker::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.02f, 0.02f});
}
} // namespace wiz
