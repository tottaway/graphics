#include "model/game_state.hh"
#include "geometry/rectangle_utils.hh"
#include "model/entity_id.hh"
#include "utility/overload.hh"
#include "utility/try.hh"
#include "view/screen.hh"
#include <strings.h>

namespace model {

GameState::GameState() {}

Result<EntityID, std::string>
GameState::add_entity(std::unique_ptr<Entity> entity) {

  if (current_entity_count_ + 1 >= max_entity_count) {
    return Err(std::string("Tried to add an entity but the maximum entity "
                           "count has already been reached"));
  }

  current_entity_count_++;

  const EntityID entity_id = EntityID{next_index_, epoch_};
  entity->entity_id_ = entity_id;
  entities_[next_index_] = (std::move(entity));

  // this must terminate because we already checked that tthe
  // current entity count wasn't too high
  while (entities_[next_index_] != nullptr) {
    next_index_++;
    if (next_index_ > max_entity_count) {
      epoch_++;
      std::cout << "new epoch " << epoch_ << std::endl;
      next_index_ = 0UL;
    }
  }
  return Ok(entity_id);
}

void GameState::remove_entity(const EntityID id) {
  if (entities_[id.index] &&
      entities_[id.index]->get_entity_id().epoch == id.epoch) {
    current_entity_count_--;
    entities_[id.index] = nullptr;
  }
}

Result<void, std::string>
GameState::advance_state(const int64_t delta_time_ns) {
  // note that updates can add new entities, we rely on entities_ being fixed
  // size to make this iteration valid
  for (const auto &entity : entities_) {
    if (entity) {
      TRY_VOID(entity->update(delta_time_ns));
    }
  }

  for (const auto &system : systems_) {
    TRY_VOID(system->update(delta_time_ns));
  }

  for (const auto &entity : entities_) {
    if (entity) {
      TRY_VOID(entity->late_update());
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
              [&entity](const view::KeyReleasedEvent &key_release)
                  -> Result<bool, std::string> {
                return entity->on_key_release(key_release);
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
  std::array<std::vector<std::size_t>, 5UL> draw_lists;
  for (const auto &entity : entities_) {
    if (entity) {
      const auto z_ordering = entity->get_z_level();
      if (z_ordering == 0U) {
        TRY_VOID(entity->draw(screen));
      } else {
        draw_lists[z_ordering - 1].emplace_back(entity->get_entity_id().index);
      }
    }
  }

  for (const auto &draw_list : draw_lists) {
    for (const auto &index : draw_list) {
      TRY_VOID(entities_[index]->draw(screen));
    }
  }
  return Ok();
}

std::optional<Entity *>
GameState::try_get_entity_pointer_by_id(const EntityID entity_id) const {
  if (entity_id.index >= entities_.size()) {
    return std::nullopt;
  }

  if (entities_[entity_id.index]) {
    if (entities_[entity_id.index]->get_entity_id().epoch != entity_id.epoch) {
      return std::nullopt;
    }
    return entities_[entity_id.index].get();
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
