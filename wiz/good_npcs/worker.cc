#include "wiz/good_npcs/worker.hh"
#include "algs/a_star.hh"
#include "geometry/rectangle_utils.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/components/character_animation_set.hh"
#include "wiz/map/grass_tile.hh"
#include "wiz/map/map.hh"
#include <ranges>

namespace wiz {
Worker::Worker(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Worker::init(const Eigen::Vector2f position) {

  position_ = position;
  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(worker_texture_set_path)));

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

  const auto random_result = dist(rng_);
  if (random_result == 1) {
    // pass in order to stick with cyan
  } else if (random_result == 2) {
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
  } else if (random_result == 3) {
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
  } else if (random_result == 4) {
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

Result<void, std::string> Worker::plan() {
  auto distance_func = [](const Eigen::Vector2i &tile_a,
                          const Eigen::Vector2i &tile_b) {
    return (tile_a - tile_b).norm();
  };

  const auto map = TRY(game_state_.get_entity_pointer_by_type<Map>());
  auto get_neighbors = [&map, this](const Eigen::Vector2i &tile_index) {
    std::array<Eigen::Vector2i, 4> side_neighbors_deltas{
        Eigen::Vector2i{0, 1},  Eigen::Vector2i{0, -1}, Eigen::Vector2i{1, 0},
        Eigen::Vector2i{-1, 0}
    };

    std::array<Eigen::Vector2i, 4> diagonal_neighbors_deltas{
        Eigen::Vector2i{1, 1},  Eigen::Vector2i{-1, -1},
        Eigen::Vector2i{-1, 1}, Eigen::Vector2i{1, -1}
    };

    algs::Neighbors<Eigen::Vector2i, 8> neighbors;
    for (const auto &neighbor_delta : side_neighbors_deltas) {
      const auto neighbor_tile_index = tile_index + neighbor_delta;
      if (map->get_map_tile_is_grass_and_has_flowers(neighbor_tile_index)) {
        neighbors.neighbor_array[neighbors.num_neighbors++] =
            neighbor_tile_index;
      }
    }

  for (const auto &neighbor_delta : diagonal_neighbors_deltas) {
      const auto neighbor_tile_index = tile_index + neighbor_delta;
      const Eigen::Vector2i adjacent_side_tile1 = {neighbor_tile_index.x(), 0};
      const Eigen::Vector2i adjacent_side_tile2 = {0, neighbor_tile_index.y()};

      uint8_t num_adjacent_side_tiles_included{0U};
      for (std::size_t i = 0; i < neighbors.num_neighbors; ++i) {
        const auto existing_neighbor = neighbors.neighbor_array[i];
        if (adjacent_side_tile1 ==  existing_neighbor ||
            adjacent_side_tile2 == existing_neighbor) {
          num_adjacent_side_tiles_included++;
        }
      }

      if (num_adjacent_side_tiles_included != 2) {
        continue;
      }

      if (map->get_map_tile_is_grass_and_has_flowers(neighbor_tile_index)) {
        neighbors.neighbor_array[neighbors.num_neighbors++] =
            neighbor_tile_index;
      }
    }

    return neighbors;
  };

  auto hueristic_function = [this](const Eigen::Vector2i &tile_index) {
    return (tile_index - goal_tile_).norm();
  };

  const auto start = map->get_tile_index_by_position(position_);
  std::cout << "Start: " << start.x() << ", " << start.y() << std::endl;
  TRY_VOID(algs::a_star<Eigen::Vector2i, 8>(
      distance_func, get_neighbors, hueristic_function, start, goal_tile_,
      500UL, maybe_current_path_on_tiles_));
  return Ok();
}

Result<void, std::string> Worker::follow_path(const int64_t delta_time_ns) {
  time_since_last_replan_ += delta_time_ns;
  if (!maybe_current_path_on_tiles_ ||
      time_since_last_replan_ > replan_delay_) {
    TRY_VOID(plan());
    time_since_last_replan_ = 0L;
  }

  const auto map = TRY(game_state_.get_entity_pointer_by_type<Map>());
  const auto current_position = map->get_tile_index_by_position(position_);
  const auto current_tile =
      TRY(map->get_map_tile_entity_by_index(current_position));
  const auto grass_tile =
      game_state_.get_entity_pointer_by_id_as<GrassTile>(current_tile).unwrap();
  if (!grass_tile->has_flowers()) {
    off_flowers_ = true;
  }

  if (current_position == goal_tile_) {
    return Ok();
  }

  if (maybe_current_path_on_tiles_->size() < 2) {
    direction_ = {0.f, 0.f};
    return Ok();
  }

  if (current_position == maybe_current_path_on_tiles_->at(1)) {
    maybe_current_path_on_tiles_->pop_front();
    if (maybe_current_path_on_tiles_->size() < 2) {
      direction_ = {0.f, 0.f};
      return Ok();
    }
  }

  const auto next_tile = maybe_current_path_on_tiles_->at(1);
  const auto next_position = map->get_tile_position_by_index(next_tile);
  direction_ = (next_position - position_).normalized().cast<float>() * speed_;
  return Ok();
}

Result<void, std::string> Worker::update(const int64_t delta_time_ns) {
  TRY_VOID(follow_path(delta_time_ns));
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
