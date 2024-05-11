#pragma once

#include "model/entity_id.hh"
#include "utility/try.hh"
#include <format>
#include <ranges>
#include <vector>

namespace model {
template <typename EntityType,
          typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int>>
Result<EntityType *, std::string>
GameState::get_entity_pointer_by_id_as(const EntityID entity_id) const {
  auto maybe_raw_pointer = try_get_entity_pointer_by_id(entity_id);
  if (!maybe_raw_pointer) {
    return Err(std::string(std::format("Entity {} does not exist", entity_id)));
  }
  auto raw_pointer = maybe_raw_pointer.value();

  if (raw_pointer->get_entity_type_name() != EntityType::entity_type_name) {
    return Err(std::string(std::format(
        "Entity {} had type {}, expected {}", entity_id,
        raw_pointer->get_entity_type_name(), EntityType::entity_type_name)));
  }
  return Ok(dynamic_cast<EntityType *>(raw_pointer));
}

template <typename EntityType,
          typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int>>
std::vector<EntityType *> GameState::get_entity_pointers_by_type() const {
  std::vector<EntityType *> result;
  for (const auto &entity : entities_) {
    if (entity) {
      if (entity->get_entity_type_name() == EntityType::entity_type_name) {
        result.emplace_back(dynamic_cast<EntityType *>(entity.get()));
      }
    }
  }
  return result;
}

template <typename EntityType,
          typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int>>
Result<EntityType *, std::string>
GameState::get_entity_pointer_by_type() const {
  const auto entity_pointers = get_entity_pointers_by_type<EntityType>();
  if (entity_pointers.empty()) {
    return Err(std::string(std::format("No entities of type {} in entity list",
                                       EntityType::entity_type_name)));
  }

  if (entity_pointers.size() > 1) {
    return Err(std::string(std::format("Expected 1 entity of type {}, found {}",
                                       EntityType::entity_type_name,
                                       entity_pointers.size())));
  }
  return Ok(entity_pointers[0]);
}

template <typename EntityType,
          typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int>>
void GameState::remove_entities_by_type() {
  for (const auto &[id, entity] : std::ranges::views::enumerate(entities_)) {
    if (entity) {
      if (entity->get_entity_type_name() == EntityType::entity_type_name) {
        remove_entity(id);
      }
    }
  }
}

template <typename EntityType, typename... Args,
          typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int>>
[[nodiscard]] Result<EntityType *, std::string>
Entity::add_child_entity(Args &&...args) {
  auto entity_result = add_child_entity_no_init<EntityType>();
  if (entity_result.isErr()) {
    return entity_result;
  }

  if constexpr (std::is_void_v<decltype(entity_result.unwrap()->init(
                    std::forward<Args>(args)...))>) {
    entity_result.unwrap()->init(std::forward<Args>(args)...);
  } else {
    const auto init_result =
        entity_result.unwrap()->init(std::forward<Args>(args)...);
    if (init_result.isErr()) {
      return Err(init_result.unwrapErr());
    }
  }
  return entity_result;
}

template <typename EntityType,
          typename std::enable_if_t<std::is_base_of_v<Entity, EntityType>, int>>
Result<EntityType *, std::string> Entity::add_child_entity_no_init() {
  auto new_entity = std::make_unique<EntityType>(game_state_);
  new_entity->set_parent_entity(get_entity_id());
  auto new_entity_raw = new_entity.get();
  const Result<EntityID, std::string> new_id =
      game_state_.add_entity(std::move(new_entity));
  child_entities_.emplace_back(TRY(new_id));
  return Ok(new_entity_raw);
}

template <typename ComponentType, typename... Args,
          typename std::enable_if_t<
              std::is_base_of_v<component::Component, ComponentType>, int>>
component::Component *Entity::add_component(Args &&...args) {
  return components_
      .emplace_back(
          std::make_unique<ComponentType>(std::forward<Args>(args)...))
      .get();
}

template <typename EntityType>
Result<EntityType *, std::string> Entity::get_parent_entity() const {
  auto maybe_parent_entity = try_get_parent_entity();
  if (!maybe_parent_entity) {
    return Err(std::string("Entity tried to get parent which didn't exist"));
  }
  auto *parent_entity = maybe_parent_entity.value();

  if (parent_entity->get_entity_type_name() != EntityType::entity_type_name) {
    return Err(std::string(std::format(
        "Parent entity was of type {}, expected {}",
        parent_entity->get_entity_type_name(), EntityType::entity_type_name)));
  }
  return Ok(dynamic_cast<EntityType *>(parent_entity));
}

} // namespace model
