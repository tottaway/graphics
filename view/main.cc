#include "ThirdParty/imgui/imgui-SFML.h"
#include "ThirdParty/imgui/imgui.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

int main() {
  sf::RenderWindow window(sf::VideoMode(640, 480), "");
  window.setVerticalSyncEnabled(true);
  ImGui::SFML::Init(window);
  sf::Color bgColor;

  float color[3] = {0.f, 0.f, 0.f};

  // let's use char array as buffer, see next part
  // for instructions on using std::string with ImGui
  char windowTitle[255] = "ImGui + SFML = <3";

  window.setTitle(windowTitle);
  window
      .resetGLStates(); // call it if you only draw ImGui. Otherwise not needed.
  sf::Clock deltaClock;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    ImGui::SFML::Update(window, deltaClock.restart());
    ImGui::Begin("Sample window"); // begin window

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetMousePos();
    ImVec2 marker_min = ImVec2(pos.x + 40, pos.y);
    ImVec2 marker_max =
        ImVec2(pos.x + 40 + 10, pos.y + ImGui::GetTextLineHeight());
    draw_list->AddRectFilled(marker_min, marker_max,
                             IM_COL32(static_cast<int>(pos.x) % 256,
                                      static_cast<int>(pos.y) % 256, 255, 255));

    // Background color edit
    if (ImGui::ColorEdit3("Background color", color)) {
      // this code gets called if color value changes, so
      // the background color is upgraded automatically!
      bgColor.r = static_cast<sf::Uint8>(color[0] * 255.f);
      bgColor.g = static_cast<sf::Uint8>(color[1] * 255.f);
      bgColor.b = static_cast<sf::Uint8>(color[2] * 255.f);
    }

    // Window title text edit
    ImGui::InputText("Window title", windowTitle, 255);

    if (ImGui::Button("Update window title")) {
      // this code gets if user clicks on the button
      // yes, you could have written if(ImGui::InputText(...))
      // but I do this to show how buttons work :)
      window.setTitle(windowTitle);
    }
    ImGui::End(); // end window

    window.clear(bgColor); // fill background with color
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}
