#include "wiz/enemies/skeleton.hh"
#include "components/animation.hh"
#include "components/collider.hh"
#include "components/hit_box.hh"
#include "components/hurt_box.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/components/character_animation_set.hh"
#include "wiz/components/health_bar.hh"
#include "wiz/components/hit_hurt_boxes.hh"
#include "wiz/map/map.hh"
#include "wiz/pathfinding/pathfinder.hh"
#include "wiz/player.hh"

namespace wiz {
Skeleton::Skeleton(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Skeleton::init(const Eigen::Vector2f position) {

  position_ = position;
  add_component<component::SolidAABBCollider>(
      [this]() { return get_transform(); },
      [this](const Eigen::Vector2f &translation) {
        this->position_ += translation;
      });

  add_component<WizHitBox<Alignement::bad>>(
      [this]() { return get_transform(); });

  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(skeleton_texture_set_path)));
  add_component<CharacterAnimationSet>(
      [this]() { return get_transform(); }, [this]() { return mode_; },
      CharacterTextureSet{texture_set->get_texture_set_by_name("idle"),
                          texture_set->get_texture_set_by_name("move_right"),
                          texture_set->get_texture_set_by_name("move_left"),
                          texture_set->get_texture_set_by_name("attack_right"),
                          texture_set->get_texture_set_by_name("attack_left"),
                          texture_set->get_texture_set_by_name("take_hit"),
                          texture_set->get_texture_set_by_name("death"), 10.f,
                          10.f, 10.f, 10.f, 10.f, 15.f, 10.f});

  add_component<HealthBar>(
      hp_, [this]() { return hp_; }, [this]() { return get_transform(); });

  add_component<WizHurtBox<Alignement::bad>>(
      [this]() { return get_transform(); }, [this]() { was_hit_ = true; });

  return Ok();
}

Result<void, std::string> Skeleton::update(const int64_t delta_time_ns) {
  // Handle character state logic
  if (mode_ == CharacterMode::dying) {
    duration_dying_hit_ns_ += delta_time_ns;
    if (duration_dying_hit_ns_ > max_duration_in_dying_ns_) {
      mode_ = CharacterMode::dead;
    }
  } else if (was_hit_ && mode_ != CharacterMode::being_hit) {
    hp_ -= 1;
    if (hp_ <= 0) {
      mode_ = CharacterMode::dying;
    } else {
      mode_ = CharacterMode::being_hit;
    }
  } else if (was_hit_) {
    duration_in_being_hit_ns_ += delta_time_ns;
    if (duration_in_being_hit_ns_ > max_duration_in_being_hit_ns_) {
      mode_ = CharacterMode::idle;
      was_hit_ = false;
      duration_in_being_hit_ns_ = 0L;
    }
  } else {
    // Use A* pathfinding to move towards player
    TRY_VOID(follow_path_to_player(delta_time_ns));

    if (direction_.x() > 0) {
      mode_ = CharacterMode::walking_right;
    } else if (direction_.x() < 0) {
      mode_ = CharacterMode::walking_left;
    } else {
      mode_ = CharacterMode::idle;
    }
  }

  if (mode_ != CharacterMode::dying && mode_ != CharacterMode::dead) {
    position_ += direction_ * (static_cast<double>(delta_time_ns) / 1e9);
  }

  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }
  return Ok();
}

Eigen::Affine2f Skeleton::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.07f, 0.1f});
}

Result<void, std::string> Skeleton::follow_path_to_player(const int64_t delta_time_ns) {
  time_since_last_replan_ns_ += delta_time_ns;

  // Get player position
  auto player = TRY(game_state_.get_entity_pointer_by_type<Player>());

  // Replan if we don't have a path or it's time to replan
  if (!maybe_current_path_on_tiles_ || time_since_last_replan_ns_ > replan_delay_ns_) {
    auto maybe_new_path = pathfinding::find_path(game_state_, position_, player->position, movement_type);
    if (maybe_new_path.isOk()) {
      maybe_current_path_on_tiles_ = std::move(maybe_new_path.unwrap());
      time_since_last_replan_ns_ = 0L;
    } else {
      // If pathfinding fails, fall back to direct movement
      direction_ = (player->position - position_).normalized() * speed_m_per_s_;
      return Ok();
    }
  }

  const auto map = TRY(game_state_.get_entity_pointer_by_type<Map>());
  const auto current_tile = map->get_tile_index_by_position(position_);
  const auto goal_tile = map->get_tile_index_by_position(player->position);

  // Check if we've reached the target
  if (current_tile == goal_tile) {
    direction_ = {0.f, 0.f};
    return Ok();
  }

  // Check if we have a valid path
  if (maybe_current_path_on_tiles_->size() < 2) {
    direction_ = {0.f, 0.f};
    return Ok();
  }

  // Remove completed waypoints
  if (current_tile == maybe_current_path_on_tiles_->at(1)) {
    maybe_current_path_on_tiles_->pop_front();
    if (maybe_current_path_on_tiles_->size() < 2) {
      direction_ = {0.f, 0.f};
      return Ok();
    }
  }

  // Move towards next waypoint
  const auto next_tile = maybe_current_path_on_tiles_->at(1);
  const auto next_position = map->get_tile_position_by_index(next_tile);
  direction_ = (next_position - position_).normalized().cast<float>() * speed_m_per_s_;

  return Ok();
}

} // namespace wiz
