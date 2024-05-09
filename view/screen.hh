#pragma once
#include "utility/try.hh"
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
  Screen();
  [[nodiscard]] Eigen::Vector2f get_mouse_pos() const;
  void start_update();
  void finish_update();
  void draw_rectangle(const Eigen::Vector2f bottom_left,
                      const Eigen::Vector2f top_right, const Color color);

  void draw_text(const Eigen::Vector2f location, const float font_size,
                 const std::string_view text);

  /// returns boolean indicating if the system is still running or an error
  [[nodiscard]] Result<bool, std::string> poll_events_and_check_for_close();
  [[nodiscard]] const std::vector<EventType> &get_events() const;
  void clear_events();

private:
  Eigen::Vector2f get_absolute_window_size() const;

  [[nodiscard]] Eigen::Vector2f
  translate_to_absolute(const Eigen::Vector2f &coordinate) const;
  sf::RenderWindow window_;
  sf::Clock delta_clock_;
  std::vector<EventType> events_;
};
} // namespace view
