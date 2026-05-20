#include "window_sdl.hpp"
#include "layout/atomicEngine/engine.hpp"
#include "layout/elements/text.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <stdexcept>

namespace ui {

SDLWindow::SDLWindow(const WindowConfig &config)
    : m_width(config.width),
      m_height(config.height) { // Cache initial dimension states

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    throw std::runtime_error("SDL3 could not be initialized! Error: " +
                             std::string(SDL_GetError()));
  }

  SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "1");

  m_window = SDL_CreateWindow(config.title.c_str(), config.width, config.height,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                                  SDL_WINDOW_HIGH_PIXEL_DENSITY);

  if (!m_window) {
    throw std::runtime_error("Window could not be created! SDL_Error: " +
                             std::string(SDL_GetError()));
  }

  SDL_SetWindowPosition(m_window, config.x, config.y);

  // Initialize backend renderer
  m_renderer = VulkanRenderer::Create(this);
  m_layoutEngine = std::make_unique<AtomicEngine>();
}

SDLWindow::~SDLWindow() {
  m_rootNode.reset();
  m_renderer.reset();

  if (m_window) {
    SDL_DestroyWindow(m_window);
  }
  SDL_Quit();
}

void SDLWindow::render() {
  if (!m_renderer || !m_layoutEngine)
    return;

  if (m_rootNode) {
    math::vec2<float> surfaceDimensions{static_cast<float>(m_width),
                                        static_cast<float>(m_height)};
    m_layoutEngine->ProcessLayout(m_rootNode.get(), surfaceDimensions);
  }

  m_renderer->begin_frame();

  const auto &processQueue = m_layoutEngine->GetRenderCache();

  for (const auto *node : processQueue) {
    const auto &metrics = node->GetLayoutMetrics();
    const ui::styleConfig *style = &node->GetStyle();

    switch (node->GetType()) {
    case ElementType::DIV:
      m_renderer->add_rect(metrics.global_position, metrics.computed_size,
                           style);
      break;

    case ElementType::TEXT: {
      const auto *textNode = static_cast<const TextElement *>(node);
      m_renderer->add_text(metrics.global_position, textNode->GetText(), style);
      break;
    }
    default:
      break;
    }
  }

  m_renderer->render_batch();
  m_renderer->end_frame();
}

void SDLWindow::poll_events() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_EVENT_QUIT:
      m_should_close = true;
      break;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      m_should_close = true;
      break;
    case SDL_EVENT_WINDOW_RESIZED: {
      int w, h;
      SDL_GetWindowSize(m_window, &w, &h);
      m_width = w;
      m_height = h;

      if (m_renderer) {
        m_renderer->on_resize(w, h);
      }
      break;
    }
    }
  }
}

void SDLWindow::swap_buffers() {}

} // namespace ui
