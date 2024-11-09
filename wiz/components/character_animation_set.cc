
#include "wiz/components/character_animation_set.hh"
namespace wiz {
CharacterAnimationSet::CharacterAnimationSet(
    GetTransformFunc get_transform, GetCharacterMode get_character_mode,
    CharacterTextureSet textures)
    : Animation(std::move(get_transform), textures.idle_textures,
                textures.idle_fps),
      character_texture_set_(textures),
      get_character_mode_(get_character_mode) {}

Result<void, std::string> CharacterAnimationSet::late_update() {
  const auto new_mode = get_character_mode_();
  if (new_mode != current_mode_) {
    current_mode_ = new_mode;
    switch (current_mode_) {
    case CharacterMode::idle: {
      update_texture_set(character_texture_set_.idle_textures,
                         character_texture_set_.idle_fps);
      break;
    }
    case CharacterMode::walking_left: {
      update_texture_set(character_texture_set_.walk_left_textures,
                         character_texture_set_.walk_left_fps);
      break;
    }
    case CharacterMode::walking_right: {
      update_texture_set(character_texture_set_.walk_right_textures,
                         character_texture_set_.walk_right_fps);
      break;
    }
    case CharacterMode::attacking_left: {
      update_texture_set(character_texture_set_.attack_left_textures,
                         character_texture_set_.attack_left_fps);
      break;
    }
    case CharacterMode::attacking_right: {
      update_texture_set(character_texture_set_.attack_right_textures,
                         character_texture_set_.attack_right_fps);
      break;
    }
    case CharacterMode::being_hit: {
      update_texture_set(character_texture_set_.hit_textures,
                         character_texture_set_.hit_fps);
      break;
    }
    case CharacterMode::dying:
      [[fallthrough]];
    case CharacterMode::dead: {
      update_texture_set(character_texture_set_.dead_textures,
                         character_texture_set_.dead_fps);
      break;
    }
    }
  }
  return Ok();
}

} // namespace wiz
