#pragma once
#include <cstdint>
#include <functional>
#include <math/vec.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace ui::font {
enum class TextStyleBit : uint8_t {
  Regular = 0,
  Bold = 1 << 0,
  Italic = 1 << 1,
};

struct TextRun {
  std::string text;
  float fontSize; // Changed to float for rich text sub-pixel tracking
  uint8_t styleFlags;
  math::vec4<float> color;
};

struct PositionedGlyph {
  math::vec4<float> rect;
  math::vec4<float> uv;
  math::vec4<float> color;
  float isText;
  float fontWeightOffset;
};

// Fixed-point quantization prevents float rounding variations from spawning
// duplicate keys
struct GlyphKey {
  char32_t codepoint;
  int32_t fixedSize;  // Font size scaled by 64.0f and converted to 26.6
                      // fixed-point representation
  uint8_t styleFlags; // Re-aligned with TextRun and implementation naming

  bool operator==(const GlyphKey &o) const {
    return codepoint == o.codepoint && fixedSize == o.fixedSize &&
           styleFlags == o.styleFlags;
  }
};

struct GlyphInfo {
  math::vec2<float> uvMin;
  math::vec2<float> uvMax;
  math::vec2<float> size;
  math::vec2<float> bearing;
  float advanceX;
};

class Font {
public:
  virtual ~Font() = default;
  virtual bool load(const std::string &path, uint32_t size) = 0;

  // Accept float size directly to pass smooth metrics down to FreeType point
  // functions
  virtual GlyphInfo getGlyphVariant(char32_t codepoint, float size,
                                    uint8_t styleFlags) = 0;

  virtual float getLineHeight(float size) const = 0;
  virtual float getAscender(float size) const = 0;

  virtual void *getTextureHandle() = 0;
};
} // namespace ui::font

namespace std {
template <> struct hash<ui::font::GlyphKey> {
  size_t operator()(const ui::font::GlyphKey &k) const {
    // Robust mixing for 26.6 keys to cleanly isolate codepoint maps across
    // sizes
    size_t h1 = hash<uint32_t>()(k.codepoint);
    size_t h2 = hash<int32_t>()(k.fixedSize);
    size_t h3 = hash<uint8_t>()(k.styleFlags);

    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};
} // namespace std
