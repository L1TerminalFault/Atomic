#pragma once
#include "layout/element.hpp"
#include "renderer/font/freetype_layout.hpp"
#include "renderer/style.hpp"
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ui {

class TextElement : public IElement {
public:
  // Updated constructor to cleanly take both style configuration and content
  explicit TextElement(const styleConfig &cfg, std::string text)
      : m_style(cfg), m_textContent(std::move(text)) {}

  ~TextElement() override = default;

  ElementType GetType() const override { return ElementType::TEXT; }

  const ui::styleConfig &GetStyle() const override { return m_style; }
  ui::styleConfig &GetStyle() override { return m_style; }

  const LayoutAccumulation &GetLayoutMetrics() const override {
    return m_geometry;
  }
  LayoutAccumulation &GetLayoutMetrics() override { return m_geometry; }

  IElement *GetParent() const override { return m_parent; }
  void SetParent(IElement *parent) override { m_parent = parent; }

  // Leaf nodes explicitly return empty structures for structural queries
  const std::vector<std::unique_ptr<IElement>> &GetChildren() const override {
    return m_emptyChildren;
  }
  void
  AddChild(std::unique_ptr<IElement> child) override { /* No-op: leaf node */ }

  const std::string &GetText() const { return m_textContent; }
  void SetText(std::string text) { m_textContent = std::move(text); }
  // Add this inside public: block of TextElement (layout/elements/text.hpp)
  math::vec2<float> ComputeIntrinsicBounds(ui::font::Font *activeFont) const {
    if (!activeFont) {
      return {0.0f, 0.0f};
    }

    // Just pass the float straight through. If it's 0.0f, calcLayout handles it
    // as infinity.
    return font::TextLayoutEngine::measureString(
        m_textContent, activeFont, m_style.fontSize, m_style.maxWidth,
        m_style.tracking);
  }

private:
  ui::styleConfig m_style;
  LayoutAccumulation m_geometry;
  IElement *m_parent = nullptr;
  std::string m_textContent;

  inline static const std::vector<std::unique_ptr<IElement>> m_emptyChildren{};
};

// Base factory variant for a single string literal or pre-built std::string
inline std::unique_ptr<IElement> Text(const styleConfig &cfg,
                                      std::string text = "") {
  return std::make_unique<TextElement>(cfg, std::move(text));
}

// Overload for cases where you might pass multiple strings, tokens, or
// variables to concatenate
template <typename... Args>
inline std::unique_ptr<IElement> Text(const styleConfig &cfg, Args &&...args) {
  // Fold expression strings append into a single stack-allocated buffer
  std::string consolidated = (std::string("") + ... + std::forward<Args>(args));
  return std::make_unique<TextElement>(cfg, std::move(consolidated));
}

} // namespace ui
