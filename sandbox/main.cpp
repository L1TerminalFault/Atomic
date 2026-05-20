#include "renderer/font/freetype_font.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include "windowing/interface.hpp"
#include "windowing/linux/window_sdl.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

int main() {
  try {
    ui::WindowConfig config{};
    config.title = "hellow wayland";
    config.width = 1200;
    config.height = 720;
    config.x = 100;
    config.y = 100;

    auto window = ui::SDLWindow::Create(config);

    ui::VulkanRenderer *renderer =
        static_cast<ui::VulkanRenderer *>(window->get_renderer());

    auto *myfont = new ui::font::FreeTypeFont();

    if (!myfont->load("inter.ttf", 32)) {
      delete myfont;
      throw std::runtime_error("coudnt find inter.ttf");
    }

    renderer->set_default_font(myfont);

    while (!window->should_close()) {
      window->poll_events();
      window->render();
    }
  } catch (const std::exception &e) {
    std::cerr << "Engine Crashed" << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
