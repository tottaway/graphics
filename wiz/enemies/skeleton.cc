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
  auto idle_textures = texture_set->get_texture_set_by_name("idle");
  add_component<component::Animation>([this]() { return get_transform(); },
                                      std::move(idle_textures), 10.f);

  add_component<HealthBar>(
      hp_, [this]() { return hp_; }, [this]() { return get_transform(); });

  add_component<WizHurtBox<Alignement::bad>>(
      [this]() { return get_transform(); }, [this]() { hp_ -= 1; });

  return Ok();
  return Ok();
}

Result<void, std::string> Skeleton::update(const int64_t delta_time_ns) {
  auto player = TRY(game_state_.get_entity_pointer_by_type<Player>());
  direction_ = (player->position - position_).normalized() * 0.5f;
  position_ += direction_ * (static_cast<double>(delta_time_ns) / 1e9);
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
