namespace wiz {

template<int64_t MapSizeX, int64_t MapSizeY>
RoomCorridorGenerator<MapSizeX, MapSizeY>::RoomCorridorGenerator(const GenerationParams& params)
    : params_(params), rng_(params.seed) {}

template<int64_t MapSizeX, int64_t MapSizeY>
typename RoomCorridorGenerator<MapSizeX, MapSizeY>::MapGrid
RoomCorridorGenerator<MapSizeX, MapSizeY>::generate() {
  MapGrid grid;
  rooms_.clear();

  std::cout << "Generating room-corridor map...\n";

  initialize_with_walls(grid);
  generate_rooms(grid);
  connect_rooms(grid);
  add_interior_walls(grid);
  ensure_border_walls(grid);

  std::cout << "Generated " << rooms_.size() << " rooms\n";

  current_grid_ = grid;
  return grid;
}

template<int64_t MapSizeX, int64_t MapSizeY>
bool RoomCorridorGenerator<MapSizeX, MapSizeY>::is_wall(int x, int y) const {
  if (x < 0 || x >= map_size_x || y < 0 || y >= map_size_y) {
    return true;
  }
  return current_grid_[x][y];
}

template<int64_t MapSizeX, int64_t MapSizeY>
const typename RoomCorridorGenerator<MapSizeX, MapSizeY>::MapGrid&
RoomCorridorGenerator<MapSizeX, MapSizeY>::get_grid() const {
  return current_grid_;
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::initialize_with_walls(MapGrid& grid) {
  for (int x = 0; x < map_size_x; ++x) {
    for (int y = 0; y < map_size_y; ++y) {
      grid[x][y] = true;
    }
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::generate_rooms(MapGrid& grid) {
  std::uniform_real_distribution<float> spawn_dist(0.0f, 1.0f);
  std::uniform_int_distribution<int> size_dist(params_.min_room_size, params_.max_room_size);

  const int area_width = map_size_x / params_.rooms_per_axis;
  const int area_height = map_size_y / params_.rooms_per_axis;

  for (int grid_x = 0; grid_x < params_.rooms_per_axis; ++grid_x) {
    for (int grid_y = 0; grid_y < params_.rooms_per_axis; ++grid_y) {
      if (spawn_dist(rng_) < params_.room_spawn_probability) {
        const int area_start_x = grid_x * area_width + 1;
        const int area_start_y = grid_y * area_height + 1;
        const int area_end_x = (grid_x + 1) * area_width - 1;
        const int area_end_y = (grid_y + 1) * area_height - 1;

        const int max_room_width = std::min(params_.max_room_size, area_end_x - area_start_x);
        const int max_room_height = std::min(params_.max_room_size, area_end_y - area_start_y);

        if (max_room_width >= params_.min_room_size && max_room_height >= params_.min_room_size) {
          std::uniform_int_distribution<int> width_dist(params_.min_room_size, max_room_width);
          std::uniform_int_distribution<int> height_dist(params_.min_room_size, max_room_height);

          const int room_width = width_dist(rng_);
          const int room_height = height_dist(rng_);

          std::uniform_int_distribution<int> x_dist(area_start_x, area_end_x - room_width);
          std::uniform_int_distribution<int> y_dist(area_start_y, area_end_y - room_height);

          const int room_x = x_dist(rng_);
          const int room_y = y_dist(rng_);

          Room room{room_x, room_y, room_width, room_height};

          bool overlaps = false;
          for (const auto& existing_room : rooms_) {
            if (rooms_overlap(room, existing_room)) {
              overlaps = true;
              break;
            }
          }

          if (!overlaps) {
            for (int x = room_x; x < room_x + room_width; ++x) {
              for (int y = room_y; y < room_y + room_height; ++y) {
                grid[x][y] = false;
              }
            }
            rooms_.push_back(room);
          }
        }
      }
    }
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::connect_rooms(MapGrid& grid) {
  if (rooms_.size() < 2) return;

  for (size_t i = 1; i < rooms_.size(); ++i) {
    create_corridor(grid, rooms_[i-1], rooms_[i]);
  }

  if (rooms_.size() > 2) {
    create_corridor(grid, rooms_.back(), rooms_.front());
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::add_interior_walls(MapGrid& grid) {
  std::uniform_real_distribution<float> wall_dist(0.0f, 1.0f);

  for (const auto& room : rooms_) {
    for (int x = room.x + 1; x < room.x + room.width - 1; ++x) {
      for (int y = room.y + 1; y < room.y + room.height - 1; ++y) {
        if (wall_dist(rng_) < params_.interior_wall_probability) {
          grid[x][y] = true;
        }
      }
    }
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::create_corridor(MapGrid& grid, const Room& from, const Room& to) {
  const int from_center_x = from.x + from.width / 2;
  const int from_center_y = from.y + from.height / 2;
  const int to_center_x = to.x + to.width / 2;
  const int to_center_y = to.y + to.height / 2;

  int current_x = from_center_x;
  int current_y = from_center_y;

  while (current_x != to_center_x) {
    grid[current_x][current_y] = false;
    current_x += (current_x < to_center_x) ? 1 : -1;
  }

  while (current_y != to_center_y) {
    grid[current_x][current_y] = false;
    current_y += (current_y < to_center_y) ? 1 : -1;
  }

  grid[to_center_x][to_center_y] = false;
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::ensure_border_walls(MapGrid& grid) {
  for (int x = 0; x < map_size_x; ++x) {
    grid[x][0] = true;
    grid[x][map_size_y - 1] = true;
  }
  for (int y = 0; y < map_size_y; ++y) {
    grid[0][y] = true;
    grid[map_size_x - 1][y] = true;
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
bool RoomCorridorGenerator<MapSizeX, MapSizeY>::rooms_overlap(const Room& a, const Room& b) const {
  return !(a.x + a.width <= b.x || b.x + b.width <= a.x ||
           a.y + a.height <= b.y || b.y + b.height <= a.y);
}

template<int64_t MapSizeX, int64_t MapSizeY>
void RoomCorridorGenerator<MapSizeX, MapSizeY>::print_map_ascii() const {
  std::cout << "\nGenerated Map (# = wall, . = walkable):\n";
  std::cout << std::string(map_size_x + 2, '-') << "\n";

  for (int y = 0; y < map_size_y; ++y) {
    std::cout << "|";
    for (int x = 0; x < map_size_x; ++x) {
      std::cout << (current_grid_[x][y] ? '#' : '.');
    }
    std::cout << "|\n";
  }

  std::cout << std::string(map_size_x + 2, '-') << "\n\n";
}

} // namespace wiz