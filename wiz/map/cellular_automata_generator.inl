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

} // namespace wiz
