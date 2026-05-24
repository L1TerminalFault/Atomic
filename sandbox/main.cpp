#include "layout/elements/div.hpp"
#include "layout/elements/text.hpp"
#include "math/vec.hpp"
#include "renderer/style.hpp"
#include "windowing/interface.hpp"
#include "windowing/linux/window_sdl.hpp"
#include <cstdlib>
#include <iostream>

using namespace ui;

int main() {
  try {
    ui::WindowConfig config{"Test 1: Complex Nested Flows", 1200, 720, 100,
                            100};
    auto window = ui::SDLWindow::Create(config);

    auto rootdiv = Div(
        {
            .pos = {200.0f, 200.0f},
            .size = {ui::SizeFit{}, ui::SizeFit{}},
            .color = {0.15f, 0.15f, 0.18f, 1.0f},
            .radius = math::vec4<float>::all(10),
            .padding = {20.0f, 40.0f, 20.0f, 40.0f},
        },
        Text(
            {// Text node shrinks tightly to its characters
             .size = {ui::SizeFit{}, ui::SizeFit{}},
             .color = {0.9f, 0.9f, 0.95f, 1.0f},
             .fontSize = 24, // Scaling test
             .tracking = 2},
            "Welcome back to our text component"));

    window->SetRootNode(std::move(rootdiv));

    while (!window->should_close()) {
      window->poll_events();
      window->render();
    }
  } catch (const std::exception &e) {
    std::cerr << "Engine Crash: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
