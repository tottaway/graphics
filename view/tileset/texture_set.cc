#include "view/tileset/texture_set.hh"
#include "yaml.h"

namespace view {
static Result<TextureSet, std::string>
parse_texture_set(const std::filesystem::path path);

}
