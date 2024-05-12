#include "view/screen.hh"
#include "ThirdParty/imgui/imconfig.h"
#include "ThirdParty/imgui/imgui-SFML.h"
#include "ThirdParty/imgui/imgui.h"
#include <GL/gl.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowStyle.hpp>

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

Screen::Screen(const Eigen::Vector2f viewport_size_m,
               const Eigen::Vector2f viewport_center)
    : window_(sf::VideoMode(640, 480), "OpenGL", sf::Style::Default,
              sf::ContextSettings(32)),
      viewport_size_m_(viewport_size_m),
      game_m_viewport_center_(viewport_center) {

  window_.setVerticalSyncEnabled(true);
  ImGui::SFML::Init(window_);
  // call it if you only draw ImGui. Otherwise not needed.
  window_.resetGLStates();

  ImGuiIO &io = ImGui::GetIO();
  fonts_.emplace_back(
      io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 128));
  ImGui::SFML::UpdateFontTexture();
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
  ImGui::SetNextWindowBgAlpha(0.f);
  ImGui::Begin("Sample window"); // begin window

  window_.clear();
  const auto window_size = ImGui::GetWindowSize();
  handle_resize({window_size.x, window_size.y});
}

void Screen::finish_update() {
  ImGui::End(); // end window
  ImGui::SFML::Render(window_);
  window_.display();
}

void Screen::draw_rectangle(const Eigen::Vector2f bottom_left,
                            const Eigen::Vector2f top_right,
                            const Color color) {
  const auto absolute_bottom_left = window_pixels_from_game_m_ * bottom_left;
  const auto absolute_top_right = window_pixels_from_game_m_ * top_right;

  glBegin(GL_QUADS);
  glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, 1.);
  glVertex2f(absolute_bottom_left.x(), absolute_bottom_left.y());
  glVertex2f(absolute_bottom_left.x(), absolute_top_right.y());
  glVertex2f(absolute_top_right.x(), absolute_top_right.y());
  glVertex2f(absolute_top_right.x(), absolute_bottom_left.y());
  glEnd();
}

void Screen::draw_rectangle(const Eigen::Vector2f bottom_left,
                            const Eigen::Vector2f top_right,
                            const Texture &texture) {
  const auto absolute_bottom_left = window_pixels_from_game_m_ * bottom_left;
  const auto absolute_top_right = window_pixels_from_game_m_ * top_right;

  sf::Texture::bind(&texture.texture_);
  glColor4f(1.0, 1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glTexCoord2f(1, 1);
  glVertex2f(absolute_bottom_left.x(), absolute_bottom_left.y());
  glTexCoord2f(1, 0);
  glVertex2f(absolute_bottom_left.x(), absolute_top_right.y());
  glTexCoord2f(0, 0);
  glVertex2f(absolute_top_right.x(), absolute_top_right.y());
  glTexCoord2f(0, 1);
  glVertex2f(absolute_top_right.x(), absolute_bottom_left.y());
  glEnd();
  sf::Texture::bind(NULL);
}

void Screen::draw_text(const Eigen::Vector2f location, const float font_size,
                       const std::string_view text, const Color color) {
  const auto absolute_location = window_pixels_from_game_m_ * location;
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  ImVec2 marker_min = ImVec2(absolute_location.x(), absolute_location.y());
  window_.pushGLStates();
  ImGui::PushFont(fonts_.front());
  draw_list->AddText(ImGui::GetFont(), font_size, marker_min,
                     IM_COL32(color.r, color.g, color.b, 255), text.data(),
                     text.data() + text.size());
  ImGui::PopFont();
  window_.popGLStates();
}

void Screen::set_viewport_center(const Eigen::Vector2f new_center) {

  viewport_m_from_game_m_ = Eigen::Translation2f{-new_center};

  window_pixels_from_game_m_ = window_pixels_from_viewport_pixels_ *
                               viewport_pixels_from_viewport_m_ *
                               viewport_m_from_game_m_;

  game_m_from_window_pixels_ = window_pixels_from_game_m_.inverse();
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
          game_m_from_window_pixels_ *
          Eigen::Vector2f{event.mouseMove.x, event.mouseMove.y}});
      break;
    }
    case sf::Event::EventType::MouseButtonPressed: {
      events_.emplace_back(MouseDownEvent{
          TRY(convert_mouse_button_enum(event.mouseButton.button)),
          game_m_from_window_pixels_ *
              Eigen::Vector2f{event.mouseButton.x, event.mouseButton.y}});
      break;
    }
    case sf::Event::EventType::MouseButtonReleased: {
      events_.emplace_back(MouseUpEvent{
          TRY(convert_mouse_button_enum(event.mouseButton.button)),
          game_m_from_window_pixels_ *
              Eigen::Vector2f{event.mouseButton.x, event.mouseButton.y}});
      break;
    }
    case sf::Event::EventType::KeyPressed: {
      events_.emplace_back(KeyPressedEvent{event.key});
      break;
    }
    case sf::Event::EventType::Resized: {
      handle_resize({event.size.width, event.size.height});
      break;
    }
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

void Screen::handle_resize(const Eigen::Vector2i new_size_pixels) {
  if (window_size_pixels_.cast<int>() == new_size_pixels) {
    return;
  }

  // save window size
  window_size_pixels_ = new_size_pixels.cast<float>();

  // determine view port size in pixels
  const float viewport_aspect_ratio =
      viewport_size_m_.x() / viewport_size_m_.y();
  const float window_aspect_ratio =
      window_size_pixels_.x() / window_size_pixels_.y();
  if (viewport_aspect_ratio > window_aspect_ratio) {
    viewport_pixels_from_viewport_m_ = Eigen::Scaling(
        window_size_pixels_.x() / viewport_size_m_.x(),
        window_size_pixels_.x() / viewport_aspect_ratio / viewport_size_m_.y());
  } else {
    viewport_pixels_from_viewport_m_ = Eigen::Scaling(
        window_size_pixels_.y() * viewport_aspect_ratio / viewport_size_m_.x(),
        window_size_pixels_.y() / viewport_size_m_.y());
  }

  // calculate viewport pixel to window pixels
  const Eigen::Matrix2f reflection_over_x_axis{{{1.0f, 0.0f}, {0.0f, -1.0f}}};
  window_pixels_from_viewport_pixels_ =
      reflection_over_x_axis *
      Eigen::Affine2f{Eigen::Translation2f{window_size_pixels_.x() / 2,
                                           -window_size_pixels_.y() / 2}};

  // calculate viewport_m_from_game_m_ TODO allow scaling and rotation
  viewport_m_from_game_m_ = Eigen::Translation2f{-game_m_viewport_center_};

  window_pixels_from_game_m_ = window_pixels_from_viewport_pixels_ *
                               viewport_pixels_from_viewport_m_ *
                               viewport_m_from_game_m_;
  game_m_from_window_pixels_ = window_pixels_from_game_m_.inverse();
}
} // namespace view
