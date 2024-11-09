#pragma once
#include "components/animation.hh"
#include "view/screen.hh"
#include "view/texture.hh"
#include "wiz/character_mode.hh"
#include <Eigen/Dense>

namespace wiz {

struct CharacterTextureSet {
  std::vector<view::Texture> idle_textures;
  std::vector<view::Texture> walk_right_textures;
  std::vector<view::Texture> walk_left_textures;
  std::vector<view::Texture> attack_right_textures;
  std::vector<view::Texture> attack_left_textures;
  std::vector<view::Texture> hit_textures;
  std::vector<view::Texture> dead_textures;
  float idle_fps;
  float walk_right_fps;
  float walk_left_fps;
  float attack_right_fps;
  float attack_left_fps;
  float hit_fps;
  float dead_fps;
};

class CharacterAnimationSet : public component::Animation {
public:
  static constexpr std::string_view component_type_name =
      "draw_character_animation_component";

  using GetTransformFunc = std::function<Eigen::Affine2f()>;
  using GetCharacterMode = std::function<CharacterMode()>;
  CharacterAnimationSet(GetTransformFunc get_transform,
                        GetCharacterMode get_character_mode,
                        CharacterTextureSet textures);

  /// Note a little hacky to do this in late update since update is handling the
  /// actual animation logic
  [[nodiscard]] virtual Result<void, std::string> late_update();

  [[nodiscard]] virtual std::string_view get_component_type_name() const {
    return component_type_name;
  }

private:
  CharacterTextureSet character_texture_set_;
  GetCharacterMode get_character_mode_;
  CharacterMode current_mode_{CharacterMode::idle};
};
} // namespace wiz
