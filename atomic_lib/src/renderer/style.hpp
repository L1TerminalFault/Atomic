#pragma once

#include "math/vec.hpp"
#include "renderer/font/interface.hpp"
#include <cstdint>
#include <variant>

namespace ui {

enum class ShapeType : uint32_t {
  RoundedRect = 0,
  Circle = 1,
  Text = 2,
  Image = 3
};

enum class FlexDirection : uint32_t { Column = 0, Row = 1 };

struct SizeFit {};
struct SizeFill {};

using Size = std::variant<float, SizeFit, SizeFill>;

inline constexpr SizeFit fit{};
inline constexpr SizeFill fill{};

struct Size2D {
  Size x = fit;
  Size y = fit;
};

struct EdgeInsets {
  float top = 0.0f;
  float right = 0.0f;
  float bottom = 0.0f;
  float left = 0.0f;

  constexpr EdgeInsets() = default;
  constexpr EdgeInsets(float t, float r, float b, float l)
      : top(t), right(r), bottom(b), left(l) {}

  static constexpr EdgeInsets all(float val) {
    return EdgeInsets(val, val, val, val);
  }
  static constexpr EdgeInsets horizontal(float val) {
    return EdgeInsets(0.0f, val, 0.0f, val);
  }
  static constexpr EdgeInsets vertical(float val) {
    return EdgeInsets(val, 0.0f, val, 0.0f);
  }

  constexpr operator math::vec4<float>() const {
    return {top, right, bottom, left};
  }
};

struct CornerRadius {
  float topLeft = 0.0f;
  float topRight = 0.0f;
  float bottomRight = 0.0f;
  float bottomLeft = 0.0f;

  constexpr CornerRadius() = default;
  constexpr CornerRadius(float tl, float tr, float br, float bl)
      : topLeft(tl), topRight(tr), bottomRight(br), bottomLeft(bl) {}

  static constexpr CornerRadius all(float val) {
    return CornerRadius(val, val, val, val);
  }
  static constexpr CornerRadius top(float val) {
    return CornerRadius(val, val, 0.0f, 0.0f);
  }
  static constexpr CornerRadius bottom(float val) {
    return CornerRadius(0.0f, 0.0f, val, val);
  }
  static constexpr CornerRadius left(float val) {
    return CornerRadius(val, 0.0f, 0.0f, val);
  }
  static constexpr CornerRadius right(float val) {
    return CornerRadius(0.0f, val, val, 0.0f);
  }

  constexpr operator math::vec4<float>() const {
    return {topLeft, topRight, bottomRight, bottomLeft};
  }
};

struct styleConfig {
  math::vec2<float> pos{0.0f, 0.0f};
  Size2D size{ui::SizeFit{}, ui::SizeFit{}};

  EdgeInsets margin;
  EdgeInsets padding;
  math::vec2<float> gap{0.0f, 0.0f};
  FlexDirection flexDirection = FlexDirection::Column;

  math::vec4<float> color = math::vec4<float>::all(1);
  CornerRadius radius;
  ShapeType shape = ShapeType::RoundedRect;

  float strokeWidth = 0.0f;
  math::vec4<float> strokeColor{0.3f, 0.3f, 0.3f, 1.0f};
  float dotGap = 0.0f;
  float dotSize = 0.0f;
  uint32_t strokePosition = 2;

  void *font = nullptr;
  int fontSize = 16;
  ui::font::TextStyleBit styleFlag = ui::font::TextStyleBit::Regular;
  int tracking = 0;
  int maxWidth = 0;

  constexpr styleConfig &SetPos(const math::vec2<float> &val) {
    pos = val;
    return *this;
  }
  constexpr styleConfig &SetSize(const Size2D &val) {
    size = val;
    return *this;
  }
  constexpr styleConfig &SetMargin(const EdgeInsets &val) {
    margin = val;
    return *this;
  }
  constexpr styleConfig &SetPadding(const EdgeInsets &val) {
    padding = val;
    return *this;
  }
  constexpr styleConfig &SetGap(const math::vec2<float> &val) {
    gap = val;
    return *this;
  }
  constexpr styleConfig &SetFlexDirection(FlexDirection val) {
    flexDirection = val;
    return *this;
  }

  constexpr styleConfig &SetColor(const math::vec4<float> &val) {
    color = val;
    return *this;
  }
  constexpr styleConfig &SetRadius(const CornerRadius &val) {
    radius = val;
    return *this;
  }
  constexpr styleConfig &SetShape(ShapeType val) {
    shape = val;
    return *this;
  }

  constexpr styleConfig &SetStrokeWidth(float val) {
    strokeWidth = val;
    return *this;
  }
  constexpr styleConfig &SetStrokeColor(const math::vec4<float> &val) {
    strokeColor = val;
    return *this;
  }
  constexpr styleConfig &SetDotGap(float val) {
    dotGap = val;
    return *this;
  }
  constexpr styleConfig &SetDotSize(float val) {
    dotSize = val;
    return *this;
  }
  constexpr styleConfig &SetStrokePosition(uint32_t val) {
    strokePosition = val;
    return *this;
  }

  constexpr styleConfig &SetFont(void *val) {
    font = val;
    return *this;
  }
  constexpr styleConfig &SetFontSize(int val) {
    fontSize = val;
    return *this;
  }
  constexpr styleConfig &SetStyleFlag(ui::font::TextStyleBit val) {
    styleFlag = val;
    return *this;
  }
  constexpr styleConfig &SetTracking(int val) {
    tracking = val;
    return *this;
  }
  constexpr styleConfig &SetMaxWidth(int val) {
    maxWidth = val;
    return *this;
  }
};

} // namespace ui
