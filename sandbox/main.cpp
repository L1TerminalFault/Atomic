#include "layout/elements/div.hpp"
#include "layout/elements/text.hpp"
#include "math/vec.hpp"
#include "renderer/style.hpp"
#include "renderer/utils/color.hpp"
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

    auto screen =
        Div(ui::styleConfig()
                .SetPadding(EdgeInsets::all(50))
                .SetStrokeWidth(0)
                .SetColor(ui::Color::transparent),
            Div(ui::styleConfig()
                    .SetColor(ui::Color::slate[900])
                    .SetMargin(EdgeInsets::horizontal(19))
                    .SetPadding(EdgeInsets::all(20))
                    .SetRadius(ui::CornerRadius::all(10))
                    .SetSize({ui::SizeFit{}, ui::SizeFit{}}),
                Text(ui::styleConfig().SetColor(ui::Color::emerald[400]),
                     "Success")));

    window->SetRootNode(std::move(screen));

    while (!window->should_close()) {
      window->poll_events();
      window->render();
    }
  } catch (const std::exception &e) {
    std::cerr << "Engine Crash: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
