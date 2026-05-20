#pragma once
#include "renderer/assetloader/interface.hpp"
#include "renderer/assetloader/types.hpp"

namespace ui::asset {

class StbAssetLoader : public AssetLoader {
public:
  StbAssetLoader() = default;
  ~StbAssetLoader() override = default;

  bool exists(const std::string &path) const override;

protected:
  // Overriding the specific asset implementation tag handles
  std::optional<ImageAsset> load_impl(internal_type_tag<ImageAsset>,
                                      const std::string &path) override;
};

} // namespace ui::asset
