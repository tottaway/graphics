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
                          texture_set->get_texture_set_by_name("death"), 15.f,
                          15.f, 15.f, 15.f, 15.f, 20.f, 15.f});

  add_component<HealthBar>(
      hp_, [this]() { return hp_; }, [this]() { return get_transform(); });

  add_component<WizHurtBox<Alignement::bad>>(
      [this]() { return get_transform(); }, [this]() { was_hit_ = true; });

  return Ok();
}

Result<void, std::string> Skeleton::update(const int64_t delta_time_ns) {
  auto player = TRY(game_state_.get_entity_pointer_by_type<Player>());
  direction_ = (player->position - position_).normalized() * 0.5f;

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
  } else if (direction_.x() > 0) {
    mode_ = CharacterMode::walking_right;
  } else {
    mode_ = CharacterMode::walking_left;
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
} // namespace wiz
