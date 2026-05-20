#pragma once

#include "renderer/assetloader/types.hpp"
#include <optional>
#include <string>
namespace ui::asset {

// asset loading interface
class AssetLoader {
public:
  virtual ~AssetLoader() = default;

  // takes in a path and loads an image from disc
  template <typename T> std::optional<T> load(const std::string &path) {
    return load_impl(internal_type_tag<T>{}, path);
  };

  virtual bool exists(const std::string &path) const = 0;

protected:
  template <typename> struct internal_type_tag {};

  virtual std::optional<struct ImageAsset>
  load_impl(internal_type_tag<struct ImageAsset>, const std::string &path) = 0;
};

} // namespace ui::asset
