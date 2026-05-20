#include "renderer/assetloader/image/stb/stb_impl.hpp"
#include "renderer/assetloader/types.hpp"
#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ui::asset {
std::optional<ui::asset::ImageAsset>
StbAssetLoader::load_impl(internal_type_tag<ImageAsset>,
                          const std::string &path) {
  if (!exists(path)) {
    std::cerr << "[Asset System] File path not found: " << path << std::endl;
  }

  int width = 0, height = 0, channels = 0;
  stbi_uc *raw_pixels =
      stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

  if (!raw_pixels) {
    std::cerr << "[Asset System] Failed to decode image: " << path << std::endl;
    return std::nullopt;
  }

  ImageAsset asset{};
  asset.width = static_cast<uint32_t>(width);
  asset.height = static_cast<uint32_t>(height);
  asset.channels = 4;

  size_t total_bytes = asset.width * asset.height * asset.channels;
  asset.pixels.assign(raw_pixels, raw_pixels + total_bytes);

  stbi_image_free(raw_pixels);
  return asset;
}

bool StbAssetLoader::exists(const std::string &path) const {
  std::ifstream file(path);
  return file.good();
}

} // namespace ui::asset
