#include "wiz/player.hh"
#include "components/animation.hh"
#include "components/center.hh"
#include "components/collider.hh"
#include "components/sprite.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include "model/game_state.hh"
#include "view/tileset/texture_set.hh"
#include "wiz/map/grass_tile.hh"

namespace wiz {
Player::Player(model::GameState &game_state) : model::Entity(game_state) {}

Result<void, std::string> Player::init() {
  // view::Texture player_texture{std::filesystem::path(player_texture_path),
  //                              Eigen::Vector2i{0, 0},
  //                              Eigen::Vector2i{100, 100}};

  add_component<component::Center>([this]() { return get_transform(); });

  add_component<component::Collider>([this](const model::EntityID &id) {
    const auto result = game_state_.get_entity_pointer_by_id_as<GrassTile>(id);
    if (result.isOk()) {
      result.unwrap()
          ->get_component<component::Collider>()
          .value()
          ->handle_collision(get_entity_id());
    }
  });

  const auto *texture_set = TRY(view::TextureSet::parse_texture_set(
      std::filesystem::path(player_texture_set_path)));
  auto idle_textures = texture_set->get_texture_set_by_name("death");

  add_component<component::Animation>([this]() { return get_transform(); },
                                      std::move(idle_textures), 10.f);

  return Ok();
}

Result<void, std::string> Player::update(const int64_t delta_time_ns) {
  position_ += (y_direction_ + x_direction_).cast<float>().normalized() *
               (static_cast<double>(delta_time_ns) / 1e9);
  for (const auto &component : components_) {
    TRY_VOID(component->update(delta_time_ns));
  }
  return Ok();
}

Result<bool, std::string>
Player::on_key_press(const view::KeyPressedEvent &key_press) {
  if (key_press.key_event.code == sf::Keyboard::W) {
    y_direction_ += Eigen::Vector2i{0, 1};
  } else if (key_press.key_event.code == sf::Keyboard::A) {
    x_direction_ += Eigen::Vector2i{-1, 0};
  } else if (key_press.key_event.code == sf::Keyboard::S) {
    y_direction_ += Eigen::Vector2i{0, -1};
  } else if (key_press.key_event.code == sf::Keyboard::D) {
    x_direction_ += Eigen::Vector2i{1, 0};
  } else {
    return Ok(true);
  }
  x_direction_.normalize();
  y_direction_.normalize();

  return Ok(false);
}

Result<bool, std::string>
Player::on_key_release(const view::KeyReleasedEvent &key_release) {
  if (key_release.key_event.code == sf::Keyboard::W) {
    y_direction_ -= Eigen::Vector2i{0, 1};
  } else if (key_release.key_event.code == sf::Keyboard::A) {
    x_direction_ -= Eigen::Vector2i{-1, 0};
  } else if (key_release.key_event.code == sf::Keyboard::S) {
    y_direction_ -= Eigen::Vector2i{0, -1};
  } else if (key_release.key_event.code == sf::Keyboard::D) {
    x_direction_ -= Eigen::Vector2i{1, 0};
  } else {
    return Ok(true);
  }

  x_direction_.normalize();
  y_direction_.normalize();

  return Ok(false);
}

Eigen::Affine2f Player::get_transform() const {
  return geometry::make_rectangle_from_center_and_size(
      position_, Eigen::Vector2f{0.1f, 0.12f});
}
} // namespace wiz
