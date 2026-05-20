#pragma once

#include <vulkan/vulkan_core.h>
namespace math {
template <typename T> struct vec2 {
  T x, y;
  template <typename U> operator vec2<U>() const {
    return {static_cast<U>(x), static_cast<U>(y)};
  }

  template <typename U> vec2<T> &operator+=(const vec2<U> &other) {
    this->x += static_cast<T>(other.x);
    this->y += static_cast<T>(other.y);
    return *this; // Fixed: Dereferenced pointer
  }

  friend vec2<T> operator+(vec2<T> lhs, const vec2<T> &rhs) {
    lhs += rhs;
    return lhs;
  }

  operator VkExtent2D() const {
    return {static_cast<uint32_t>(x), static_cast<uint32_t>(y)};
  }
};
template <typename T> struct vec3 {
  T x, y, z;
};
template <typename T> struct vec4 {
  T x, y, z, w;
  template <typename U> operator vec4<U>() const {
    return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z),
            static_cast<U>(w)};
  }
};
} // namespace math
