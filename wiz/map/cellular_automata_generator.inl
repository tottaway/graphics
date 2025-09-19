namespace wiz {

template<int64_t MapSizeX, int64_t MapSizeY>
CellularAutomataGenerator<MapSizeX, MapSizeY>::CellularAutomataGenerator(const GenerationParams& params)
    : params_(params), rng_(params.seed) {}

template<int64_t MapSizeX, int64_t MapSizeY>
typename CellularAutomataGenerator<MapSizeX, MapSizeY>::MapGrid
CellularAutomataGenerator<MapSizeX, MapSizeY>::generate() {
  auto grid = initialize_with_noise();

  for (int i = 0; i < params_.smoothing_iterations; ++i) {
    smooth_iteration(grid);
  }

  ensure_border_walls(grid);
  widen_narrow_passages(grid);
  ensure_connectivity(grid);
  current_grid_ = grid;
  return grid;
}

template<int64_t MapSizeX, int64_t MapSizeY>
bool CellularAutomataGenerator<MapSizeX, MapSizeY>::is_wall(int x, int y) const {
  if (x < 0 || x >= map_size_x || y < 0 || y >= map_size_y) {
    return true;
  }
  return current_grid_[x][y];
}

template<int64_t MapSizeX, int64_t MapSizeY>
const typename CellularAutomataGenerator<MapSizeX, MapSizeY>::MapGrid&
CellularAutomataGenerator<MapSizeX, MapSizeY>::get_grid() const {
  return current_grid_;
}

template<int64_t MapSizeX, int64_t MapSizeY>
typename CellularAutomataGenerator<MapSizeX, MapSizeY>::MapGrid
CellularAutomataGenerator<MapSizeX, MapSizeY>::initialize_with_noise() {
  MapGrid grid;
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);

  for (int x = 0; x < map_size_x; ++x) {
    for (int y = 0; y < map_size_y; ++y) {
      grid[x][y] = dist(rng_) < params_.initial_wall_probability;
    }
  }

  return grid;
}

template<int64_t MapSizeX, int64_t MapSizeY>
void CellularAutomataGenerator<MapSizeX, MapSizeY>::smooth_iteration(MapGrid& grid) {
  MapGrid new_grid = grid;

  for (int x = 1; x < map_size_x - 1; ++x) {
    for (int y = 1; y < map_size_y - 1; ++y) {
      int wall_neighbors = count_wall_neighbors(grid, x, y);
      new_grid[x][y] = wall_neighbors >= params_.wall_neighbor_threshold;
    }
  }

  grid = new_grid;
}

template<int64_t MapSizeX, int64_t MapSizeY>
int CellularAutomataGenerator<MapSizeX, MapSizeY>::count_wall_neighbors(const MapGrid& grid, int x, int y) {
  int count = 0;
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      int nx = x + dx;
      int ny = y + dy;

      if (nx < 0 || nx >= map_size_x || ny < 0 || ny >= map_size_y) {
        count++;
      } else if (grid[nx][ny]) {
        count++;
      }
    }
  }
  return count;
}

template<int64_t MapSizeX, int64_t MapSizeY>
void CellularAutomataGenerator<MapSizeX, MapSizeY>::ensure_border_walls(MapGrid& grid) {
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
void CellularAutomataGenerator<MapSizeX, MapSizeY>::widen_narrow_passages(MapGrid& grid) {
  // Find narrow passages and create 2x2 clearances
  for (int x = 1; x < map_size_x - 1; ++x) {
    for (int y = 1; y < map_size_y - 1; ++y) {
      if (!grid[x][y] && is_narrow_passage(grid, x, y)) {
        // Try to create a 2x2 clearance around this tile
        // Prefer the configuration that requires clearing the fewest walls

        int best_config = -1;
        int min_walls_to_clear = 5; // Maximum possible walls to clear in a 2x2

        // Check each 2x2 configuration and count walls that need clearing
        // Configuration 1: This tile is top-left of 2x2
        if (x < map_size_x - 1 && y < map_size_y - 1) {
          int walls_to_clear = (grid[x][y] ? 1 : 0) + (grid[x+1][y] ? 1 : 0) +
                              (grid[x][y+1] ? 1 : 0) + (grid[x+1][y+1] ? 1 : 0);
          if (walls_to_clear < min_walls_to_clear) {
            min_walls_to_clear = walls_to_clear;
            best_config = 1;
          }
        }

        // Configuration 2: This tile is top-right of 2x2
        if (x > 0 && y < map_size_y - 1) {
          int walls_to_clear = (grid[x-1][y] ? 1 : 0) + (grid[x][y] ? 1 : 0) +
                              (grid[x-1][y+1] ? 1 : 0) + (grid[x][y+1] ? 1 : 0);
          if (walls_to_clear < min_walls_to_clear) {
            min_walls_to_clear = walls_to_clear;
            best_config = 2;
          }
        }

        // Configuration 3: This tile is bottom-left of 2x2
        if (x < map_size_x - 1 && y > 0) {
          int walls_to_clear = (grid[x][y-1] ? 1 : 0) + (grid[x+1][y-1] ? 1 : 0) +
                              (grid[x][y] ? 1 : 0) + (grid[x+1][y] ? 1 : 0);
          if (walls_to_clear < min_walls_to_clear) {
            min_walls_to_clear = walls_to_clear;
            best_config = 3;
          }
        }

        // Configuration 4: This tile is bottom-right of 2x2
        if (x > 0 && y > 0) {
          int walls_to_clear = (grid[x-1][y-1] ? 1 : 0) + (grid[x][y-1] ? 1 : 0) +
                              (grid[x-1][y] ? 1 : 0) + (grid[x][y] ? 1 : 0);
          if (walls_to_clear < min_walls_to_clear) {
            min_walls_to_clear = walls_to_clear;
            best_config = 4;
          }
        }

        // Clear walls for the best configuration (avoid clearing border walls)
        if (best_config == 1 && x < map_size_x - 2 && y < map_size_y - 2) {
          grid[x][y] = false; grid[x+1][y] = false;
          grid[x][y+1] = false; grid[x+1][y+1] = false;
        } else if (best_config == 2 && x > 1 && y < map_size_y - 2) {
          grid[x-1][y] = false; grid[x][y] = false;
          grid[x-1][y+1] = false; grid[x][y+1] = false;
        } else if (best_config == 3 && x < map_size_x - 2 && y > 1) {
          grid[x][y-1] = false; grid[x+1][y-1] = false;
          grid[x][y] = false; grid[x+1][y] = false;
        } else if (best_config == 4 && x > 1 && y > 1) {
          grid[x-1][y-1] = false; grid[x][y-1] = false;
          grid[x-1][y] = false; grid[x][y] = false;
        }
      }
    }
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
bool CellularAutomataGenerator<MapSizeX, MapSizeY>::is_narrow_passage(const MapGrid& grid, int x, int y) const {
  // Check if this walkable tile has inadequate clearance for entity movement
  if (grid[x][y]) return false; // Not a walkable tile

  // For proper movement in all 8 directions, check if we can form a 2x2 block
  // of walkable space with this tile as one of the corners

  // Check all four possible 2x2 configurations where this tile is a corner
  bool has_valid_2x2 = false;

  // Configuration 1: This tile is top-left of 2x2
  if (x < map_size_x - 1 && y < map_size_y - 1) {
    if (!grid[x][y] && !grid[x+1][y] && !grid[x][y+1] && !grid[x+1][y+1]) {
      has_valid_2x2 = true;
    }
  }

  // Configuration 2: This tile is top-right of 2x2
  if (x > 0 && y < map_size_y - 1) {
    if (!grid[x-1][y] && !grid[x][y] && !grid[x-1][y+1] && !grid[x][y+1]) {
      has_valid_2x2 = true;
    }
  }

  // Configuration 3: This tile is bottom-left of 2x2
  if (x < map_size_x - 1 && y > 0) {
    if (!grid[x][y-1] && !grid[x+1][y-1] && !grid[x][y] && !grid[x+1][y]) {
      has_valid_2x2 = true;
    }
  }

  // Configuration 4: This tile is bottom-right of 2x2
  if (x > 0 && y > 0) {
    if (!grid[x-1][y-1] && !grid[x][y-1] && !grid[x-1][y] && !grid[x][y]) {
      has_valid_2x2 = true;
    }
  }

  // If no valid 2x2 configuration exists, this is a narrow passage
  return !has_valid_2x2;
}

template<int64_t MapSizeX, int64_t MapSizeY>
void CellularAutomataGenerator<MapSizeX, MapSizeY>::print_map_ascii() const {
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

template<int64_t MapSizeX, int64_t MapSizeY>
void CellularAutomataGenerator<MapSizeX, MapSizeY>::ensure_connectivity(MapGrid& grid) {
  auto blocks = find_valid_2x2_blocks(grid);
  if (blocks.empty()) return;

  std::cout << "Found " << blocks.size() << " valid 2x2 blocks\n";

  // Find connected components using block-based connectivity
  std::set<std::pair<int32_t, int32_t>> visited;
  std::vector<std::vector<std::pair<int32_t, int32_t>>> components;

  for (const auto& block : blocks) {
    if (visited.find(block) != visited.end()) continue;

    // Start new connected component
    std::vector<std::pair<int32_t, int32_t>> component;
    std::queue<std::pair<int32_t, int32_t>> queue;

    queue.push(block);
    visited.insert(block);

    while (!queue.empty()) {
      auto current = queue.front();
      queue.pop();
      component.push_back(current);

      // Check all other unvisited blocks for connectivity
      for (const auto& other : blocks) {
        if (visited.find(other) == visited.end() &&
            blocks_are_connected(grid, current.first, current.second, other.first, other.second)) {
          visited.insert(other);
          queue.push(other);
        }
      }
    }

    components.push_back(component);
  }

  std::cout << "Found " << components.size() << " connected components\n";
  for (size_t i = 0; i < components.size(); ++i) {
    std::cout << "  Component " << i << ": " << components[i].size() << " blocks\n";
  }

  // Create corridors to connect all components to the largest one
  if (components.size() > 1) {
    // Find the largest component (assume it's the main one)
    size_t largest_idx = 0;
    for (size_t i = 1; i < components.size(); ++i) {
      if (components[i].size() > components[largest_idx].size()) {
        largest_idx = i;
      }
    }

    std::cout << "Connecting all components to component " << largest_idx << " (largest)\n";

    // Connect each other component to the largest one
    for (size_t i = 0; i < components.size(); ++i) {
      if (i != largest_idx) {
        std::cout << "  Creating corridor from component " << i << " to " << largest_idx << "\n";
        create_connecting_corridor(grid, components[i], components[largest_idx]);
      }
    }

    std::cout << "Corridor creation completed\n";
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
std::vector<std::pair<int32_t, int32_t>>
CellularAutomataGenerator<MapSizeX, MapSizeY>::find_valid_2x2_blocks(const MapGrid& grid) const {
  std::vector<std::pair<int32_t, int32_t>> blocks;

  for (int32_t x = 0; x < map_size_x - 1; ++x) {
    for (int32_t y = 0; y < map_size_y - 1; ++y) {
      // Check if 2x2 block starting at (x,y) is all walkable
      if (!grid[x][y] && !grid[x+1][y] && !grid[x][y+1] && !grid[x+1][y+1]) {
        blocks.push_back({x, y});
      }
    }
  }

  return blocks;
}

template<int64_t MapSizeX, int64_t MapSizeY>
bool CellularAutomataGenerator<MapSizeX, MapSizeY>::blocks_are_connected(
    const MapGrid& grid, int32_t x1, int32_t y1, int32_t x2, int32_t y2) const {
  // Quick geometric check first
  int32_t dx = abs(x2 - x1);
  int32_t dy = abs(y2 - y1);
  if (dx > 1 || dy > 1 || (dx == 0 && dy == 0)) {
    return false;
  }

  // Check if there's a valid movement path between any tiles in the blocks
  for (int32_t dx1 = 0; dx1 < 2; ++dx1) {
    for (int32_t dy1 = 0; dy1 < 2; ++dy1) {
      for (int32_t dx2 = 0; dx2 < 2; ++dx2) {
        for (int32_t dy2 = 0; dy2 < 2; ++dy2) {
          int32_t from_x = x1 + dx1, from_y = y1 + dy1;
          int32_t to_x = x2 + dx2, to_y = y2 + dy2;

          if (is_valid_entity_move(grid, from_x, from_y, to_x, to_y)) {
            return true;
          }
        }
      }
    }
  }

  return false;
}

template<int64_t MapSizeX, int64_t MapSizeY>
bool CellularAutomataGenerator<MapSizeX, MapSizeY>::is_valid_entity_move(
    const MapGrid& grid, int32_t from_x, int32_t from_y, int32_t to_x, int32_t to_y) const {
  int32_t dx = to_x - from_x;
  int32_t dy = to_y - from_y;

  // Only allow single-step moves (8-directional)
  if (abs(dx) > 1 || abs(dy) > 1) return false;
  if (dx == 0 && dy == 0) return false;

  // Destination must be walkable
  if (to_x < 0 || to_x >= map_size_x || to_y < 0 || to_y >= map_size_y) return false;
  if (grid[to_x][to_y]) return false;

  // For diagonal moves, check that we don't cut corners through walls
  if (dx != 0 && dy != 0) {
    // Can't move diagonally if either adjacent cardinal direction is blocked
    if (grid[from_x + dx][from_y] || grid[from_x][from_y + dy]) {
      return false;
    }
  }

  return true;
}

template<int64_t MapSizeX, int64_t MapSizeY>
void CellularAutomataGenerator<MapSizeX, MapSizeY>::create_connecting_corridor(
    MapGrid& grid,
    const std::vector<std::pair<int32_t, int32_t>>& from_component,
    const std::vector<std::pair<int32_t, int32_t>>& to_component) {

  // Find the closest blocks between the two components
  auto [from_block, to_block] = find_closest_blocks(from_component, to_component);

  std::cout << "    Connecting block (" << from_block.first << "," << from_block.second
            << ") to (" << to_block.first << "," << to_block.second << ")\n";

  // Create a 2-tile wide corridor between the blocks
  // Use L-shaped path: horizontal first, then vertical
  int32_t start_x = from_block.first + 1; // Center of 2x2 block
  int32_t start_y = from_block.second + 1;
  int32_t end_x = to_block.first + 1;
  int32_t end_y = to_block.second + 1;

  // Horizontal segment
  int32_t current_x = start_x;
  while (current_x != end_x) {
    int32_t step = (current_x < end_x) ? 1 : -1;
    current_x += step;

    // Clear 2x2 area centered on current position
    for (int32_t dx = -1; dx <= 0; ++dx) {
      for (int32_t dy = -1; dy <= 0; ++dy) {
        int32_t clear_x = current_x + dx;
        int32_t clear_y = start_y + dy;
        if (clear_x > 0 && clear_x < map_size_x - 1 &&
            clear_y > 0 && clear_y < map_size_y - 1) {
          grid[clear_x][clear_y] = false;
        }
      }
    }
  }

  // Vertical segment
  int32_t current_y = start_y;
  while (current_y != end_y) {
    int32_t step = (current_y < end_y) ? 1 : -1;
    current_y += step;

    // Clear 2x2 area centered on current position
    for (int32_t dx = -1; dx <= 0; ++dx) {
      for (int32_t dy = -1; dy <= 0; ++dy) {
        int32_t clear_x = end_x + dx;
        int32_t clear_y = current_y + dy;
        if (clear_x > 0 && clear_x < map_size_x - 1 &&
            clear_y > 0 && clear_y < map_size_y - 1) {
          grid[clear_x][clear_y] = false;
        }
      }
    }
  }
}

template<int64_t MapSizeX, int64_t MapSizeY>
std::pair<std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>>
CellularAutomataGenerator<MapSizeX, MapSizeY>::find_closest_blocks(
    const std::vector<std::pair<int32_t, int32_t>>& component1,
    const std::vector<std::pair<int32_t, int32_t>>& component2) const {

  int32_t min_distance = INT32_MAX;
  std::pair<int32_t, int32_t> closest1, closest2;

  for (const auto& block1 : component1) {
    for (const auto& block2 : component2) {
      // Calculate Manhattan distance between block centers
      int32_t dx = abs((block1.first + 1) - (block2.first + 1));
      int32_t dy = abs((block1.second + 1) - (block2.second + 1));
      int32_t distance = dx + dy;

      if (distance < min_distance) {
        min_distance = distance;
        closest1 = block1;
        closest2 = block2;
      }
    }
  }

  return {closest1, closest2};
}

} // namespace wiz