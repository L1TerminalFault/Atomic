#pragma once

#include <cstdint>
#include <vector>
namespace ui::asset {
enum class AssetType { IMAGE, FONT, AUDIO, VIDEO };

struct ImageAsset {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t channels = 0;
  std::vector<uint8_t> pixels;
};
} // namespace ui::asset
