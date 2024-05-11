#pragma once
#include "utility/try.hh"

namespace systems {
class System {
public:
  virtual ~System() = default;

  virtual Result<void, std::string> update(const int64_t delta_time_ns) = 0;

  virtual std::string_view get_system_type_name() const = 0;
};
} // namespace systems
