#pragma once
#include "ThirdParty/imgui/imgui.h"
#include "utility/try.hh"
#include "view/texture.hh"
#include <Eigen/Dense>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <variant>

namespace view {

struct Color {
  int r;
  int g;
  int b;
};

/// All support event types, doesn't not include:
enum class MouseButton {
  Left,     //!< The left mouse button
  Right,    //!< The right mouse button
  Middle,   //!< The middle (wheel) mouse button
  XButton1, //!< The first extra mouse button
  XButton2, //!< The second extra mouse button
};

struct MouseDownEvent {
  MouseButton button;
  Eigen::Vector2f position;
};

struct MouseUpEvent {
  MouseButton button;
  Eigen::Vector2f position;
};

struct MouseMovedEvent {
  Eigen::Vector2f position;
};

struct KeyPressedEvent {
  sf::Event::KeyEvent key_event;
};

using EventType = std::variant<MouseUpEvent, MouseMovedEvent, MouseDownEvent,
                               KeyPressedEvent>;

class Screen {
public:
  Screen(const Eigen::Vector2f viewport_size_m = {2.0f, 2.0f},
         const Eigen::Vector2f viewport_center = {.0f, .0f});
  [[nodiscard]] Eigen::Vector2f get_mouse_pos() const;
  void start_update();
  void finish_update();
  void draw_rectangle(const Eigen::Vector2f bottom_left,
                      const Eigen::Vector2f top_right, const Color color);

  void draw_rectangle(const Eigen::Vector2f bottom_left,
                      const Eigen::Vector2f top_right, const Texture &texture);

  void draw_text(const Eigen::Vector2f location, const float font_size,
                 const std::string_view text, const Color color);

  void set_viewport_center(const Eigen::Vector2f new_center);

  /// returns boolean indicating if the system is still running or an error
  [[nodiscard]] Result<bool, std::string> poll_events_and_check_for_close();
  [[nodiscard]] const std::vector<EventType> &get_events() const;
  void clear_events();

private:
  void handle_resize(const Eigen::Vector2i new_size);

  Eigen::Vector2f get_window_size_pixels() const;

  sf::RenderWindow window_;
  sf::Clock delta_clock_;
  std::vector<EventType> events_;
  std::vector<ImFont *> fonts_;

  /// Size of the full window in pixels
  Eigen::Vector2f window_size_pixels_;

  // Size of the viewport in pixels
  Eigen::Vector2f viewport_size_pixels_;

  /// transform from the viewport (measured in pixels with 0,0 at the center of
  /// the viewport)
  Eigen::Affine2f window_pixels_from_viewport_pixels_;

  Eigen::Affine2f viewport_pixels_from_viewport_m_;

  Eigen::Affine2f viewport_m_from_game_m_;

  Eigen::Vector2f viewport_size_m_;

  Eigen::Vector2f game_m_viewport_center_{0.f, 0.f};

  Eigen::Affine2f window_pixels_from_game_m_;

  Eigen::Affine2f game_m_from_window_pixels_;
};
} // namespace view
