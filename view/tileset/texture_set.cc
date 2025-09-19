#include "view/tileset/texture_set.hh"
#include "yaml-cpp/yaml.h"
#include <ranges>
#include <stdexcept>

namespace view {
std::unordered_map<std::string, TextureSet> TextureSet::s_texture_set_cache{};

Result<TextureSet *, std::string>
TextureSet::parse_texture_set(const std::filesystem::path path) {
  const auto find_result = s_texture_set_cache.find(path.string());
  if (find_result != s_texture_set_cache.end()) {
    return Ok(&find_result->second);
  }
  try {
    TextureSet texture_set;
    const auto node = YAML::LoadFile(path);
    for (const auto &file_node : node) {
      const auto image_file_name = file_node["file_name"].as<std::string>();
      const auto subsections = file_node["subsections"];
      for (YAML::const_iterator it = subsections.begin();
           it != subsections.end(); ++it) {
        const auto subsection_name = it->first.as<std::string>();
        const auto subsection_node = it->second;
        const Eigen::Vector2i start{
            subsection_node["start"][0].as<int>(),
            subsection_node["start"][1].as<int>(),
        };
        const Eigen::Vector2i tile_size{
            subsection_node["tile_size"][0].as<int>(),
            subsection_node["tile_size"][1].as<int>(),
        };
        const Eigen::Vector2i padding{
            subsection_node["padding"][0].as<int>(),
            subsection_node["padding"][1].as<int>(),
        };
        for (const auto i : std::ranges::views::iota(
                 int32_t{0}, subsection_node["horizontal_tile_count"].as<int32_t>())) {
          for (const auto j : std::ranges::views::iota(
                   int32_t{0}, subsection_node["vertical_tile_count"].as<int32_t>())) {
            const Eigen::Vector2i tile_start =
                start + tile_size.cwiseProduct(Eigen::Vector2i{i, j});
            const Eigen::Vector2i tile_end = tile_start + tile_size;

            Eigen::Vector2i bottom_left;
            Eigen::Vector2i top_right;
            if (subsection_node["reflect_x_axis"]) {
              bottom_left = {tile_start.x() + padding.x(),
                             tile_start.y() + padding.y()};
              top_right = {tile_end.x() - padding.x(),
                           tile_end.y() - padding.y()};
            } else {
              bottom_left = {tile_end.x() - padding.x(),
                             tile_start.y() + padding.y()};
              top_right = {tile_start.x() + padding.x(),
                           tile_end.y() - padding.y()};
            }
            texture_set.texture_sets_[subsection_name].emplace_back(
                image_file_name, bottom_left, top_right);
          }
        }
      }
    }
    s_texture_set_cache.emplace(path.string(), texture_set);
    return Ok(&s_texture_set_cache[path.string()]);
  } catch (const std::runtime_error exception) {
    return Err(std::string(exception.what()));
  }
}

std::vector<Texture> TextureSet::get_texture_set_by_name(
    const std::string_view sequence_name) const {
  return texture_sets_.find(std::string(sequence_name))->second;
}
} // namespace view
