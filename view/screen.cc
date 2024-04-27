#include "view/screen.hh"
#include "ThirdParty/imgui/imgui-SFML.h"
#include "ThirdParty/imgui/imgui.h"
#include <SFML/Window/Event.hpp>

namespace view {
namespace {
Result<MouseButton, std::string>
convert_mouse_button_enum(const sf::Mouse::Button mouse_button) {
  switch (mouse_button) {
  case sf::Mouse::Button::Left:
    return Ok(MouseButton::Left);
  case sf::Mouse::Button::Right:
    return Ok(MouseButton::Right);
  case sf::Mouse::Button::Middle:
    return Ok(MouseButton::Middle);
  case sf::Mouse::Button::XButton1:
    return Ok(MouseButton::XButton1);
  case sf::Mouse::Button::XButton2:
    return Ok(MouseButton::XButton2);
  case sf::Mouse::Button::ButtonCount:
    return Err(std::string("Unexpected mouse button, ButtonCount"));
  }
}
} // namespace

CoordinateVector make_relative_coordinate(const float x, const float y) {
  return CoordinateVector{CoordinateType::relative, Eigen::Vector2f{x, y}};
}

CoordinateVector make_relative_coordinate(const Eigen::Vector2f vec) {
  return CoordinateVector{CoordinateType::relative, vec};
}

CoordinateVector make_absolute_coordinate(const float x, const float y) {
  return CoordinateVector{CoordinateType::absolute, Eigen::Vector2f{x, y}};
}

CoordinateVector make_absolute_coordinate(const Eigen::Vector2f vec) {
  return CoordinateVector{CoordinateType::absolute, vec};
}

Position make_relative_position(const float x, const float y) {
  return Position{make_relative_coordinate(x, y)};
}

Position make_relative_position(const Eigen::Vector2f vec) {
  return Position{make_relative_coordinate(vec)};
}

Position make_absolute_position(const float x, const float y) {
  return Position{make_absolute_coordinate(x, y)};
}

Position make_absolute_position(const Eigen::Vector2f vec) {
  return Position{make_absolute_coordinate(vec)};
}

Size make_relative_size(const float x, const float y) {
  return Size{make_relative_coordinate(x, y)};
}

Size make_relative_size(const Eigen::Vector2f vec) {
  return Size{make_relative_coordinate(vec)};
}

Size make_absolute_size(const float x, const float y) {
  return Size{make_absolute_coordinate(x, y)};
}

Size make_absolute_size(const Eigen::Vector2f vec) {
  return Size{make_absolute_coordinate(vec)};
}

Box make_relative_box(const float bottom_left_x, const float bottom_left_y,
                      const float size_x, const float size_y) {
  return Box{make_relative_position(bottom_left_x, bottom_left_y),
             make_relative_size(size_x, size_y)};
}

Box make_absolute_box(const float bottom_left_x, const float bottom_left_y,
                      const float size_x, const float size_y) {
  return Box{make_absolute_position(bottom_left_x, bottom_left_y),
             make_absolute_size(size_x, size_y)};
}

Screen::Screen() : window_(sf::VideoMode(640, 480), "") {
  window_.setVerticalSyncEnabled(true);
  ImGui::SFML::Init(window_);
  // call it if you only draw ImGui. Otherwise not needed.
  window_.resetGLStates();
}

Eigen::Vector2f Screen::get_mouse_pos() const {
  ImVec2 pos = ImGui::GetMousePos();
  return {pos.x, pos.y};
}

void Screen::start_update() {
  ImGui::SFML::Update(window_, delta_clock_.restart());
  const auto io = ImGui::GetIO();
  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
  ImGui::Begin("Sample window"); // begin window
}

void Screen::finish_update() {
  ImGui::End(); // end window
  ImGui::SFML::Render(window_);
  window_.display();
}

void Screen::draw_rectangle(const Box &box, const Color color) {

  const auto absolute_bottom_left = translate_to_absolute(box.bottom_left);
  const auto absolute_size = translate_to_absolute(box.size);
  const auto absolute_top_right = absolute_bottom_left + absolute_size;

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  ImVec2 marker_min =
      ImVec2(absolute_bottom_left.x(), absolute_bottom_left.y());
  ImVec2 marker_max = ImVec2(absolute_top_right.x(), absolute_top_right.y());
  draw_list->AddRectFilled(marker_min, marker_max,
                           IM_COL32(color.r, color.b, color.g, 255));
}

void Screen::draw_text(const Position location, const float font_size,
                       const std::string_view text) {
  const auto absolute_location = translate_to_absolute(location);
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  ImVec2 marker_min = ImVec2(absolute_location.x(), absolute_location.y());
  draw_list->AddText(ImGui::GetFont(), font_size, marker_min,
                     IM_COL32(255, 255, 255, 255), text.data(),
                     text.data() + text.size());
}

Result<bool, std::string> Screen::poll_events_and_check_for_close() {
  sf::Event event;
  if (!window_.isOpen()) {
    return Ok(false);
  }

  while (window_.pollEvent(event)) {
    ImGui::SFML::ProcessEvent(event);

    switch (event.type) {
    case sf::Event::EventType::Closed: {
      window_.close();
      return Ok(false);
    }
    case sf::Event::EventType::MouseMoved: {
      events_.emplace_back(MouseMovedEvent{
          make_absolute_position(event.mouseMove.x, event.mouseMove.y)});
      break;
    }
    case sf::Event::EventType::MouseButtonPressed: {
      events_.emplace_back(MouseDownEvent{
          TRY(convert_mouse_button_enum(event.mouseButton.button)),
          make_absolute_position(event.mouseButton.x, event.mouseButton.y)});
      break;
    }
    case sf::Event::EventType::MouseButtonReleased: {
      events_.emplace_back(MouseUpEvent{
          TRY(convert_mouse_button_enum(event.mouseButton.button)),
          make_absolute_position(event.mouseButton.x, event.mouseButton.y)});
      break;
    }
    case sf::Event::EventType::KeyPressed: {
      events_.emplace_back(KeyPressedEvent{event.key});
      break;
    }
    case sf::Event::EventType::Resized:
    case sf::Event::EventType::LostFocus:
    case sf::Event::EventType::GainedFocus:
    case sf::Event::EventType::TextEntered:
    case sf::Event::EventType::KeyReleased:
    case sf::Event::EventType::MouseWheelMoved:
    case sf::Event::EventType::MouseWheelScrolled:
    case sf::Event::EventType::MouseEntered:
    case sf::Event::EventType::MouseLeft:
    case sf::Event::EventType::JoystickButtonPressed:
    case sf::Event::EventType::JoystickButtonReleased:
    case sf::Event::EventType::JoystickMoved:
    case sf::Event::EventType::JoystickConnected:
    case sf::Event::EventType::JoystickDisconnected:
    case sf::Event::EventType::TouchBegan:
    case sf::Event::EventType::TouchMoved:
    case sf::Event::EventType::TouchEnded:
    case sf::Event::EventType::SensorChanged: {
      // Unsupported Event type, this should turn into an error
      continue;
    }
    case sf::Event::EventType::Count: {
      return Err(
          std::string("Received Count event which should not be possibe"));
    }
    }
  }
  return Ok(true);
}

const std::vector<EventType> &Screen::get_events() const { return events_; }

void Screen::clear_events() { events_.clear(); }

bool Screen::check_if_position_is_in_box(const Position pos,
                                         const Box box) const {
  const auto absolute_pos = translate_to_absolute(pos);
  const auto absolute_box_bottom_left = translate_to_absolute(box.bottom_left);
  const auto absolute_size = translate_to_absolute(box.size);
  return !(absolute_pos.x() < absolute_box_bottom_left.x() ||
           absolute_pos.x() >
               absolute_box_bottom_left.x() + absolute_size.x() ||
           absolute_pos.y() < absolute_box_bottom_left.y() ||
           absolute_pos.y() > absolute_box_bottom_left.y() + absolute_size.y());
}

Eigen::Vector2f Screen::get_absolute_window_size() const {
  const auto window_size = ImGui::GetWindowSize();
  return Eigen::Vector2f{window_size.x, window_size.y};
}

Eigen::Vector2f Screen::translate_to_absolute(const Position &position) const {
  return translate_to_absolute(position.coord, true);
}
Eigen::Vector2f Screen::translate_to_absolute(const Size &size) const {
  return translate_to_absolute(size.coord, false);
}

Eigen::Vector2f
Screen::translate_to_absolute(const CoordinateVector &coordinate,
                              const bool translate) const {
  switch (coordinate.type) {
  case CoordinateType::absolute:
    return coordinate.vec;
  case CoordinateType::relative:
    const auto window_size = get_absolute_window_size();
    Eigen::Vector2f scaled =
        std::min(window_size.x(), window_size.y()) * coordinate.vec;
    if (translate) {
      if (window_size.x() <= window_size.y()) {
        scaled.y() += ((window_size.y() - window_size.x()) / 2);
      } else {
        scaled.x() += ((window_size.x() - window_size.y()) / 2);
      }
    }
    return scaled;
  }
}
} // namespace view
