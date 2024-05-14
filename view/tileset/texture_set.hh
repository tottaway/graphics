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
  static Result<TextureSet, std::string>
  parse_texture_set(const std::filesystem::path path);

  /// Look up textures in the set by the name specified in a yaml file
  Texture get_static_texture_by_name(const std::string_view texture_name);

  std::vector<Texture>
  get_texture_sequence_by_name(const std::string_view sequence_name);

private:
  std::unordered_map<std::string, Texture> textures_;

  std::unordered_map<std::string, std::vector<Texture>> texture_sequences_;
};

} // namespace view
