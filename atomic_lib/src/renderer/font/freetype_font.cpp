#include "renderer/font/freetype_font.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace ui::font {

// Define and initialize static member variable boundaries safely
bool FreeTypeFont::s_libInited = false;
FT_Library FreeTypeFont::s_library = nullptr;

bool FreeTypeFont::initFreeType() {
  if (!s_libInited) {
    if (FT_Init_FreeType(&s_library) != 0) {
      throw std::runtime_error("Failed to init FreeTypeFont library instance!");
    }
    s_libInited = true;
  }
  return s_libInited;
}

bool FreeTypeFont::load(const std::string &path, uint32_t size) {
  if (!s_libInited) {
    initFreeType();
  }

  if (FT_New_Face(s_library, path.c_str(), 0, &m_face) != 0) {
    throw std::runtime_error("Failed to load font file at path: " + path);
  }

  // Set default pixel dimensions matching configuration
  FT_Set_Pixel_Sizes(m_face, 0, size);

  // FreeType uses 26.6 fixed-point numbers for dimensions (1/64th of a pixel)
  m_lineHeight = static_cast<float>(m_face->size->metrics.height) / 64.0f;
  m_ascender = static_cast<float>(m_face->size->metrics.ascender) / 64.0f;

  // Initialize flat single-channel (R8) pixel tracking array canvas
  m_atlasPixels.assign(m_atlasWidth * m_atlasHeight, 0);

  // Setup basic row-packing pen coordinates with 2px padding margins
  m_nextPackX = 2;
  m_nextPackY = 2;
  m_maxRowHeight = 0;
  m_textureDirty = true;

  // Warm up the glyph cache cache with printable ASCII range immediately
  generateAtlas();

  return true;
}

GlyphInfo FreeTypeFont::getGlyphVariant(char32_t codepoint, uint32_t size,
                                        uint8_t styleFlags) {
  GlyphKey key{codepoint, size, styleFlags};

  auto it = m_glyphCache.find(key);
  if (it != m_glyphCache.end()) {
    return it->second;
  }

  return rasterizeAndPackGlyph(codepoint, size, styleFlags);
}

GlyphInfo FreeTypeFont::rasterizeAndPackGlyph(char32_t codepoint, uint32_t size,
                                              uint8_t styleFlags) {
  // Ensure the internal FreeType font face state is targeting the correct size
  FT_Set_Pixel_Sizes(m_face, 0, size);

  // Trigger rasterizer to populate m_face->glyph->bitmap with a single-channel
  // alpha mask
  if (FT_Load_Char(m_face, codepoint, FT_LOAD_RENDER) != 0) {
    throw std::runtime_error(
        "Failed to render glyph character via FreeType context!");
  }

  FT_Bitmap &bitmap = m_face->glyph->bitmap;
  uint32_t glyphWidth = bitmap.width;
  uint32_t glyphRows = bitmap.rows;

  // Shelf packing strategy: if we exceed row limits, drop down to the next text
  // line
  if (m_nextPackX + glyphWidth + 2 >= m_atlasWidth) {
    m_nextPackX = 2;
    m_nextPackY += m_maxRowHeight + 2;
    m_maxRowHeight = 0;
  }

  // Safeguard against absolute texture sheet limits
  if (m_nextPackY + glyphRows + 2 >= m_atlasHeight) {
    throw std::runtime_error(
        "Vulkan UI Error: Font Atlas texture filled to maximum dimensions!");
  }

  m_maxRowHeight = std::max(m_maxRowHeight, glyphRows);

  // Blit FreeType's raw grayscale buffer pixels into our local linear atlas
  // canvas sheet
  for (uint32_t row = 0; row < glyphRows; ++row) {
    uint32_t destY = m_nextPackY + row;
    uint32_t destX = m_nextPackX;

    std::memcpy(&m_atlasPixels[destY * m_atlasWidth + destX],
                &bitmap.buffer[row * bitmap.width], glyphWidth);
  }

  // Generate normalized UV coordinates maps matching shader vertex
  // interpolations
  GlyphInfo info{};
  info.uvMin = {
      static_cast<float>(m_nextPackX) / static_cast<float>(m_atlasWidth),
      static_cast<float>(m_nextPackY) / static_cast<float>(m_atlasHeight)};
  info.uvMax = {static_cast<float>(m_nextPackX + glyphWidth) /
                    static_cast<float>(m_atlasWidth),
                static_cast<float>(m_nextPackY + glyphRows) /
                    static_cast<float>(m_atlasHeight)};

  // Extract dimensions and structural metrics to pass out to TextLayoutEngine
  info.size = {static_cast<float>(glyphWidth), static_cast<float>(glyphRows)};
  info.bearing = {static_cast<float>(m_face->glyph->bitmap_left),
                  static_cast<float>(m_face->glyph->bitmap_top)};
  info.advanceX =
      static_cast<float>(m_face->glyph->advance.x >>
                         6); // Transform 26.6 to standard pixel offsets

  // Store inside lookup map layout
  GlyphKey key{codepoint, size, styleFlags};
  m_glyphCache[key] = info;

  // Advance packing cursor rightward, including safe 2px spacing to prevent
  // texel bleeding
  m_nextPackX += glyphWidth + 2;
  m_textureDirty = true;

  return info;
}

bool FreeTypeFont::consumeTextureDirtyBit() {
  bool dirty = m_textureDirty;
  m_textureDirty = false;
  return dirty;
}

void FreeTypeFont::generateAtlas() {
  // Populate our lookup tables with the basic ASCII character space on initial
  // load
  for (char c = 32; c < 127; ++c) {
    rasterizeAndPackGlyph(static_cast<char32_t>(c), 24, 0);
  }
}

} // namespace ui::font
