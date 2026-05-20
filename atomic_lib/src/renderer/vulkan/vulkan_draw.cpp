
#include "renderer/font/freetype_layout.hpp"
#include "renderer/font/interface.hpp"
#include "renderer/style.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ui {

void VulkanRenderer::add_rect(float x, float y, float w, float h,
                              ui::styleConfig *style) {
  UIInstance instance;
  instance.pos = {x, y};
  instance.size = {w, h};
  instance.color = style->color;
  instance.radius = style->radius;
  instance.shapeType = static_cast<uint32_t>(style->shape);
  instance.strokeWidth = style->strokeWidth;
  instance.strokeColor = style->strokeColor;
  instance.dotGap = style->dotGap;
  instance.dotSize = style->dotSize;
  instance.strokePosition = style->strokePosition;
  m_ui_queue.push_back(instance);
}

void VulkanRenderer::add_circle(float x, float y, float radius,
                                ui::styleConfig *style) {
  style->radius = {radius, radius, radius, radius};
  add_rect(x, y, radius * 2, radius * 2, style);
}

void VulkanRenderer::add_text(float x, float y, const std::string &text,
                              ui::styleConfig *style) {
  if (!style->font)
    return;

  std::vector<font::TextRun> runs = font::TextLayoutEngine::parseRichText(
      text, style->fontSize, style->color);

  if (!runs.empty()) {
    runs[0].styleFlags = static_cast<uint8_t>(style->styleFlag);
  }

  std::vector<ui::font::PositionedGlyph> PositionedGlyph =
      font::TextLayoutEngine::calcLayout(
          runs, static_cast<ui::font::Font *>(style->font), style->maxWidth,
          style->tracking);

  for (const auto &pg : PositionedGlyph) {
    UIInstance instance{};
    instance.pos = {x + pg.rect.x, y + pg.rect.y};
    instance.size = {pg.rect.z, pg.rect.w};
    instance.color = pg.color;

    instance.shapeType = 2;
    instance.uvMin = {pg.uv.x, pg.uv.y};
    instance.uvMax = {pg.uv.z, pg.uv.w};

    // HACK: repurposing stroke width to font weight offset is not ideal
    instance.strokeWidth = pg.fontWeightOffset;

    m_ui_queue.push_back(instance);
  }
}

} // namespace ui
