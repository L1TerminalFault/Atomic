#pragma once
#include "layout/element.hpp"

namespace ui {

class TextElement : public IElement {
public:
  explicit TextElement(std::string text) : m_textContent(std::move(text)) {}
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

private:
  ui::styleConfig m_style;
  LayoutAccumulation m_geometry;
  IElement *m_parent = nullptr;
  std::string m_textContent;

  inline static const std::vector<std::unique_ptr<IElement>> m_emptyChildren{};
};

} // namespace ui
