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

void Screen::draw_rectangle(const Eigen::Vector2f bottom_left,
                            const Eigen::Vector2f top_right,
                            const Color color) {

  const auto absolute_bottom_left = translate_to_absolute(bottom_left);
  const auto absolute_top_right = translate_to_absolute(top_right);

  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  ImVec2 marker_min =
      ImVec2(absolute_bottom_left.x(), absolute_bottom_left.y());
  ImVec2 marker_max = ImVec2(absolute_top_right.x(), absolute_top_right.y());
  draw_list->AddRectFilled(marker_min, marker_max,
                           IM_COL32(color.r, color.b, color.g, 255));
}

void Screen::draw_text(const Eigen::Vector2f location, const float font_size,
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
      events_.emplace_back(
          MouseMovedEvent{{event.mouseMove.x, event.mouseMove.y}});
      break;
    }
    case sf::Event::EventType::MouseButtonPressed: {
      events_.emplace_back(MouseDownEvent{
          TRY(convert_mouse_button_enum(event.mouseButton.button)),
          {event.mouseButton.x, event.mouseButton.y}});
      break;
    }
    case sf::Event::EventType::MouseButtonReleased: {
      events_.emplace_back(
          MouseUpEvent{TRY(convert_mouse_button_enum(event.mouseButton.button)),
                       {event.mouseButton.x, event.mouseButton.y}});
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

Eigen::Vector2f Screen::get_absolute_window_size() const {
  const auto window_size = ImGui::GetWindowSize();
  return Eigen::Vector2f{window_size.x, window_size.y};
}

Eigen::Vector2f
Screen::translate_to_absolute(const Eigen::Vector2f &coordinate) const {
  const auto window_size = get_absolute_window_size();
  const Eigen::Vector2f flip_and_scale{0.5, 0.5};
  const Eigen::Vector2f translate{1.0f, 1.0f};
  const Eigen::Vector2f relative_screen_coord =
      flip_and_scale.cwiseProduct(coordinate + translate);
  Eigen::Vector2f scaled =
      std::min(window_size.x(), window_size.y()) * relative_screen_coord;
  if (window_size.x() <= window_size.y()) {
    scaled.y() += ((window_size.y() - window_size.x()) / 2);
  } else {
    scaled.x() += ((window_size.x() - window_size.y()) / 2);
  }
  return scaled;
}
} // namespace view
