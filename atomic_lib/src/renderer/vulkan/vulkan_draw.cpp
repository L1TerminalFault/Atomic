#include "renderer/font/freetype_layout.hpp"
#include "renderer/font/interface.hpp"
#include "renderer/style.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ui {

void VulkanRenderer::add_rect(const math::vec2<float> &globalPosition,
                              const math::vec2<float> &computedSize,
                              const ui::styleConfig *style) {
  if (!style)
    return;

  UIInstance instance{};
  instance.pos = globalPosition;
  instance.size = computedSize;
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

void VulkanRenderer::add_circle(const math::vec2<float> &globalPosition,
                                float radius, ui::styleConfig *style) {
  if (!style)
    return;

  style->radius = {radius, radius, radius, radius};
  math::vec2<float> diameterSize{radius * 2.0f, radius * 2.0f};

  add_rect(globalPosition, diameterSize, style);
}

void VulkanRenderer::add_text(const math::vec2<float> &globalPosition,
                              const std::string &text,
                              const ui::styleConfig *style) {
  if (!style) {
    printf("no font so not rendering");
    return;
  }
  ui::font::Font *activeFont = style->font
                                   ? static_cast<ui::font::Font *>(style->font)
                                   : m_default_font.get();

  if (!activeFont) {
    std::cerr << "Renderer Warning: Dropping text draw call due to missing "
                 "Font asset."
              << std::endl;
    return;
  }

  std::vector<font::TextRun> runs = font::TextLayoutEngine::parseRichText(
      text, style->fontSize, style->color);

  if (!runs.empty()) {
    runs[0].styleFlags = static_cast<uint8_t>(style->styleFlag);
  }

  std::vector<ui::font::PositionedGlyph> positionedGlyphs =
      font::TextLayoutEngine::calcLayout(runs, activeFont, style->maxWidth,
                                         style->tracking);

  float fontAscender = activeFont->getAscender();

  for (const auto &pg : positionedGlyphs) {
    UIInstance instance{};
    // Apply local glyph offsets relative to the parent element's global layout
    // position
    instance.pos = {globalPosition.x + pg.rect.x,
                    globalPosition.y + fontAscender + pg.rect.y};
    instance.size = {pg.rect.z, pg.rect.w};
    instance.color = pg.color;

    instance.shapeType = 2; // SHAPE_TEXT
    instance.uvMin = {pg.uv.x, pg.uv.y};
    instance.uvMax = {pg.uv.z, pg.uv.w};

    // HACK: repurposing stroke width to font weight offset is not ideal
    instance.strokeWidth = pg.fontWeightOffset;

    m_ui_queue.push_back(instance);
  }
}

void VulkanRenderer::add_image(const math::vec2<float> &globalPosition,
                               const math::vec2<float> &computedSize,
                               const std::string &path,
                               const ui::styleConfig *style) {
  uint32_t textureId = get_or_create_texture(path);

  UIInstance instance{};
  instance.pos = globalPosition;
  instance.size = computedSize;
  instance.color =
      style ? style->color : math::vec4<float>{1.0f, 1.0f, 1.0f, 1.0f};
  instance.radius =
      style ? style->radius : math::vec4<float>{0.0f, 0.0f, 0.0f, 0.0f};

  instance.shapeType = 3; // SHAPE_IMAGE
  instance.textureIndex = textureId;

  instance.uvMin = {0.0f, 0.0f};
  instance.uvMax = {1.0f, 1.0f};

  instance.strokeWidth = style ? style->strokeWidth : 0.0f;
  instance.strokeColor =
      style ? style->strokeColor : math::vec4<float>{0.0f, 0.0f, 0.0f, 0.0f};
  instance.strokePosition = style ? style->strokePosition : 0;
  instance.dotGap = style ? style->dotGap : 0.0f;
  instance.dotSize = style ? style->dotSize : 0.0f;

  m_ui_queue.push_back(instance);
}

} // namespace ui
