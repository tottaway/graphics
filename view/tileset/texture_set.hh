#pragma once
#include "utility/try.hh"
#include "view/texture.hh"
#include <filesystem>
#include <unordered_map>

namespace view {

/// Class representing a collection of textures
class TextureSet {
public:
  /// Parse texture set from a file
  ///
  /// @param[in] path yaml file describing a texture set
  /// @return errors if yaml file doesn't exists or contains errors
  static Result<TextureSet *, std::string>
  parse_texture_set(const std::filesystem::path path);

  /// TODO: error
  std::vector<Texture>
  get_texture_sequence_by_name(const std::string_view sequence_name) const;

  std::vector<Texture>
  get_texture_set_by_name(const std::string_view sequence_name) const;

private:
  std::unordered_map<std::string, std::vector<Texture>> texture_sequences_;

  std::unordered_map<std::string, std::vector<Texture>> texture_sets_;

  static std::unordered_map<std::string, TextureSet> s_texture_set_cache;
};

} // namespace view
