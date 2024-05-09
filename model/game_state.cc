#include "model/game_state.hh"
#include "geometry/rectangle_utils.hh"
#include "utility/overload.hh"
#include "utility/try.hh"
#include "view/screen.hh"
#include <strings.h>

namespace model {

GameState::GameState() { entities_.reserve(max_entity_count); }

Result<EntityID, std::string>
GameState::add_entity(std::unique_ptr<Entity> entity) {
  if (entities_.size() > max_entity_count) {
    return Err(std::string("Tried to add an entity but the maximum entity "
                           "count has already been reached"));
  }
  const auto entity_id = static_cast<EntityID>(entities_.size());
  entity->entity_id_ = entity_id;
  entities_.emplace_back(std::move(entity));
  return Ok(entity_id);
}

void GameState::remove_entity(const EntityID id) { entities_[id] = nullptr; }

Result<void, std::string>
GameState::advance_state(const int64_t delta_time_ns) {
  // note that updates can add new entities, we rely on entities_ being fixed
  // size to make this iteration valid
  for (const auto &entity : entities_) {
    if (entity) {
      TRY_VOID(entity->update(delta_time_ns));
    }
  }
  return Ok();
}

Result<void, std::string> GameState::handle_event(const view::EventType &event,
                                                  const view::Screen &screen) {
  for (const auto &entity : entities_) {
    if (entity) {
      const auto should_continue = TRY(std::visit(
          utility::Overload{
              [this, &entity, &screen](const view::MouseUpEvent &mouse_up)
                  -> Result<bool, std::string> {
                return handle_mouse_up_for_entity(*entity, mouse_up, screen);
              },
              [&entity](const view::KeyPressedEvent &key_press)
                  -> Result<bool, std::string> {
                return entity->on_key_press(key_press);
              },
              [](const auto &) -> Result<bool, std::string> {
                return Ok(true);
              },
          },
          event));

      if (!should_continue) {
        break;
      }
    }
  }
  return Ok();
}

Result<bool, std::string>
GameState::handle_mouse_up_for_entity(Entity &entity,
                                      const view::MouseUpEvent &mouse_up,
                                      const view::Screen &screen) {
  if (geometry::rectangle_contains_point(entity.get_transform(),
                                         mouse_up.position)) {
    return entity.on_click(mouse_up);
  }
  return Ok(true);
}

Result<void, std::string> GameState::draw(view::Screen &screen) const {
  for (const auto &entity : entities_) {
    if (entity) {
      TRY_VOID(entity->draw(screen));
    }
  }
  return Ok();
}

std::optional<Entity *>
GameState::try_get_entity_pointer_by_id(const EntityID entity_id) const {
  if (entity_id >= entities_.size()) {
    return std::nullopt;
  }

  if (entities_[entity_id]) {
    return entities_[entity_id].get();
  }
  return std::nullopt;
}

Entity::Entity(GameState &game_state) : game_state_(game_state) {}

Result<void, std::string> Entity::draw(view::Screen &screen) const {
  for (const auto &component : components_) {
    TRY_VOID(component->draw(screen));
  }
  return Ok();
}

EntityID Entity::get_entity_id() const { return entity_id_; };

void Entity::remove_entity(const EntityID entity_id) {
  game_state_.remove_entity(entity_id);
  std::ranges::remove(child_entities_, entity_id);
}

void Entity::remove_child_entities() {
  const auto child_entities_copy = child_entities_;
  for (const auto child_id : child_entities_copy) {
    remove_entity(child_id);
  }
}

std::optional<Entity *> Entity::try_get_parent_entity() const {
  if (maybe_parent_entity_) {
    return game_state_.try_get_entity_pointer_by_id(
        maybe_parent_entity_.value());
  }
  return std::nullopt;
}

Entity::~Entity() {
  remove_child_entities();
  auto maybe_parent_entity = try_get_parent_entity();
  if (maybe_parent_entity) {
    std::ranges::remove(maybe_parent_entity.value()->child_entities_,
                        entity_id_);
  }
}

void Entity::set_parent_entity(const EntityID parent_entity_id) {
  maybe_parent_entity_ = parent_entity_id;
}
} // namespace model
