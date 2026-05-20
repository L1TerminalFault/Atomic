#pragma once

#include "math/vec.hpp"
#include "renderer/font/interface.hpp"
#include <cstdint>

namespace ui {

enum class ShapeType : uint32_t {
  RoundedRect = 0,
  Circle = 1,
  Text = 2,
  Image = 3
};

enum class FlexDirection : uint32_t { Column = 0, Row = 1 };

struct styleConfig {
  math::vec2<float> pos;
  math::vec2<float> size;
  math::vec4<float> color;
  math::vec4<float> radius;
  ShapeType shape = ShapeType::RoundedRect;

  FlexDirection flexDirection = FlexDirection::Column;
  math::vec4<float> margin = {0.0f, 0.0f, 0.0f,
                              0.0f}; // Ordered: [Top, Right, Bottom, Left]
  math::vec4<float> padding = {0.0f, 0.0f, 0.0f,
                               0.0f}; // Ordered: [Top, Right, Bottom, Left]
  math::vec2<float> gap = {0.0f, 0.0f};

  float strokeWidth = 1.0f;
  math::vec4<float> strokeColor = {0.3f, 0.3f, 0.3f, 1.0f};
  float dotGap = 0.0f;
  float dotSize = 0.0f;
  uint32_t strokePosition = 2; // 0-inner, 1-center, 2-out

  void *font = nullptr;
  int fontSize = 16;
  ui::font::TextStyleBit styleFlag;
  int tracking = 0;
  int maxWidth = 0;
};

} // namespace ui
