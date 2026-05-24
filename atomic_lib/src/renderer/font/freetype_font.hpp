#pragma once
#include <cstdint>
#include <ft2build.h>
#include <renderer/font/interface.hpp>
#include <vector>
#include FT_FREETYPE_H
#include <unordered_map>

namespace ui::font {
class FreeTypeFont : public Font {
private:
  FT_Face m_face = nullptr;
  static FT_Library s_library;
  static bool s_libInited;

  // Cache uses the newly quantized fixed-point GlyphKey struct
  std::unordered_map<GlyphKey, GlyphInfo> m_glyphCache;
  void *m_vulkanTextureHandle = nullptr;

  float m_lineHeight = 0.0f;
  float m_ascender = 0.0f;

  std::vector<uint8_t> m_atlasPixels;
  uint32_t m_atlasWidth = 1024;
  uint32_t m_atlasHeight = 1024;

  // Coordinates kept at uint32_t to cleanly track physical atlas slot offsets
  uint32_t m_nextPackX;
  uint32_t m_nextPackY;
  uint32_t m_maxRowHeight;
  bool m_textureDirty; // Fixed standard type signature from float to bool

  void generateAtlas();
  uint32_t m_initialSize{24};

public:
  static bool initFreeType();
  FreeTypeFont() { initFreeType(); }
  ~FreeTypeFont() {
    if (m_face) {
      FT_Done_Face(m_face);
    }
  }

  bool consumeTextureDirtyBit();
  bool load(const std::string &path, uint32_t size) override;

  // Signatures cleanly updated to match Font virtual interface overrides
  GlyphInfo getGlyphVariant(char32_t codepoint, float size,
                            uint8_t styleFlags) override;

  float getLineHeight(float size) const override;
  float getAscender(float size) const override;

  const uint8_t *getRawPixels() const { return m_atlasPixels.data(); }
  uint32_t getAtlasWidth() const { return m_atlasWidth; }
  uint32_t getAtlasHeight() const { return m_atlasHeight; }
  void *getTextureHandle() override { return m_vulkanTextureHandle; };

  // Match implementation signatures to take float sizes
  GlyphInfo rasterizeAndPackGlyph(char32_t codepoint, float size,
                                  uint8_t styleFlags);
};
} // namespace ui::font
