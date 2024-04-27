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

enum class CoordinateType { absolute, relative };

struct CoordinateVector {
  CoordinateType type;
  Eigen::Vector2f vec;
};

[[nodiscard]] CoordinateVector make_relative_coordinate(const float x,
                                                        const float y);
[[nodiscard]] CoordinateVector
make_relative_coordinate(const Eigen::Vector2f vec);
[[nodiscard]] CoordinateVector make_absolute_coordinate(const float x,
                                                        const float y);
[[nodiscard]] CoordinateVector
make_absolute_coordinate(const Eigen::Vector2f vec);

struct Position {
  CoordinateVector coord;
};

[[nodiscard]] Position make_relative_position(const float x, const float y);
[[nodiscard]] Position make_relative_position(const Eigen::Vector2f vec);
[[nodiscard]] Position make_absolute_position(const float x, const float y);
[[nodiscard]] Position make_absolute_position(const Eigen::Vector2f vec);

struct Size {
  CoordinateVector coord;
};

[[nodiscard]] Size make_relative_size(const float x, const float y);
[[nodiscard]] Size make_relative_size(const Eigen::Vector2f vec);
[[nodiscard]] Size make_absolute_size(const float x, const float y);
[[nodiscard]] Size make_absolute_size(const Eigen::Vector2f vec);

struct Box {
  Position bottom_left;
  Size size;
};

[[nodiscard]] Box make_relative_box(const float bottom_left_x,
                                    const float bottom_left_y,
                                    const float size_x, const float size_y);
[[nodiscard]] Box make_absolute_box(const float bottom_left_x,
                                    const float bottom_left_y,
                                    const float size_x, const float size_y);

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
  Position position;
};

struct MouseUpEvent {
  MouseButton button;
  Position position;
};

struct MouseMovedEvent {
  Position position;
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
  void draw_rectangle(const Box &box, const Color color);

  void draw_text(const Position location, const float font_size,
                 const std::string_view text);

  /// returns boolean indicating if the system is still running or an error
  [[nodiscard]] Result<bool, std::string> poll_events_and_check_for_close();
  [[nodiscard]] const std::vector<EventType> &get_events() const;
  void clear_events();

  [[nodiscard]] bool check_if_position_is_in_box(const Position pos,
                                                 const Box box) const;

private:
  [[nodiscard]] Eigen::Vector2f
  translate_to_absolute(const Position &position) const;
  [[nodiscard]] Eigen::Vector2f translate_to_absolute(const Size &size) const;
  [[nodiscard]] Eigen::Vector2f
  translate_to_absolute(const CoordinateVector &coordinate,
                        const bool translate) const;
  [[nodiscard]] Eigen::Vector2f get_absolute_window_size() const;
  sf::RenderWindow window_;
  sf::Clock delta_clock_;
  std::vector<EventType> events_;
};
} // namespace view
