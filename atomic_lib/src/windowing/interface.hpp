#pragma once
#include "layout/atomicEngine/engine.hpp"
#include "layout/element.hpp"
#include "renderer/interface.hpp"
#include <memory>
#include <string>

namespace ui {

struct WindowConfig {
  std::string title = "atomic window default title";
  int width = 1200;
  int height = 720;
  int x = 0;
  int y = 0;
  bool vsync = true;
};

class Window {
public:
  virtual ~Window() = default;

  virtual void poll_events() = 0;
  virtual void swap_buffers() = 0;
  virtual bool should_close() = 0;
  virtual void render() = 0;

  virtual void *get_native_handle() const = 0;
  virtual float get_dpi_scale() { return 1.0f; };

  void SetRootNode(std::unique_ptr<IElement> root) {
    m_rootNode = std::move(root);
  }
  IElement *GetRootNode() const { return m_rootNode.get(); }

  Renderer *get_renderer() { return m_renderer.get(); }

protected:
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<ui::AtomicEngine> m_layoutEngine;
  std::unique_ptr<IElement> m_rootNode;
};

} // namespace ui
