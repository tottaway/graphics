#pragma once

#include "utility/try.hh"
#include <cstdint>
#include <strings.h>

namespace model {
struct EntityID {
  bool operator==(const EntityID &other) const {
    return (index == other.index) && (epoch == other.epoch);
  }
  // index of this entity in the game state vector
  std::size_t index{0UL};
  // since indexes can be reused, we differentiate by checking the epoch
  uint64_t epoch{0UL};
};
} // namespace model
