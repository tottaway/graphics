#include "wiz/map/grass_tile.hh"
#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/enemies/skeleton.hh"
#include "wiz/player.hh"
#include <chrono>
#include <iostream>
#include <random>

namespace wiz {
GrassTile::GrassTile(model::GameState &game_state)
    : model::Entity(game_state) {}

Result<void, std::string> GrassTile::init(const Eigen::Vector2f position) {

  add_component<component::NonCollidableAABBCollider>(
      [this]() { return get_transform(); },
      [this](const model::EntityID &entity_id) mutable {
        const auto maybe_player =
            game_state_.get_entity_pointer_by_id_as<Player>(entity_id);
        if (maybe_player.isOk()) {
          has_player_ = true;
        }

        const auto maybe_skeleton =
            game_state_.get_entity_pointer_by_id_as<Skeleton>(entity_id);
        if (maybe_skeleton.isOk()) {
          has_bad_guy_ = true;
        }
      });

  position_ = position;

  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(texture_set_path)));
  std::random_device dev;
  std::mt19937 rng(dev());

  const auto grass_texture_set =
      texture_set->get_texture_set_by_name(grass_texture_name);
  const auto flower_texture_set =
      texture_set->get_texture_set_by_name(flower_texture_name);
  const auto tree_texture_set =
      texture_set->get_texture_set_by_name(tree_texture_name);

  std::uniform_int_distribution<std::mt19937::result_type> grass_dist(
      0, grass_texture_set.size() - 1);
  maybe_grass_texture_.emplace(grass_texture_set[grass_dist(rng)]);
  std::uniform_int_distribution<std::mt19937::result_type> flower_dist(
      0, flower_texture_set.size() - 1);
  maybe_flower_texture_.emplace(texture_set->get_texture_set_by_name(
      flower_texture_name)[flower_dist(rng)]);
  std::uniform_int_distribution<std::mt19937::result_type> tree_dist(
      0, tree_texture_set.size() - 1);
  maybe_tree_texture_.emplace(
      texture_set->get_texture_set_by_name(tree_texture_name)[tree_dist(rng)]);

  transform_ = geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.05f, 0.05f});

  add_component<component::Sprite>([this]() {
    return component::Sprite::SpriteInfo{
        transform_, has_flowers_ ? maybe_flower_texture_.value()
                                 : maybe_grass_texture_.value()};
  });

  std::uniform_int_distribution<std::mt19937::result_type> has_tree_dist(0,
                                                                         100);
  if (has_tree_dist(rng) == 0) {
    TRY(add_child_entity_and_init<Tree>(position_,
                                        maybe_tree_texture_.value()));
  };
  return Ok();
}

Result<void, std::string> GrassTile::update(const int64_t) {
  if (has_player_) {
    has_flowers_ = true;
    has_player_ = false;
  } else if (has_bad_guy_) {
    has_flowers_ = false;
    has_bad_guy_ = false;
  }
  return Ok();
}

Eigen::Affine2f GrassTile::get_transform() const { return transform_; }

Tree::Tree(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Tree::init(const Eigen::Vector2f position,
                                     view::Texture tree_texture) {
  transform_ = geometry::make_rectangle_from_center_and_size(
                   position, Eigen::Vector2f{0.05f, 0.05f})
                   .translate(Eigen::Vector2f{0.0, 1.5})
                   .scale(Eigen::Vector2f{1.5, 2.});
  add_component<component::Sprite>([this, tree_texture]() {
    return component::Sprite::SpriteInfo{transform_, tree_texture};
  });
  return Ok();
}

Eigen::Affine2f Tree::get_transform() const { return transform_; }

} // namespace wiz
