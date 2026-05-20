#pragma once
#include "layout/element.hpp"
#include "renderer/style.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace ui {

class DivElement : public IElement {
public:
  DivElement() = default;
  ~DivElement() override = default;

  ElementType GetType() const override { return ElementType::DIV; }
  const ui::styleConfig &GetStyle() const override { return m_style; }
  ui::styleConfig &GetStyle() override { return m_style; }

  const ui::LayoutAccumulation &GetLayoutMetrics() const override {
    return m_geometry;
  }
  ui::LayoutAccumulation &GetLayoutMetrics() override { return m_geometry; }

  IElement *GetParent() const override { return m_parent; }
  void SetParent(IElement *parent) override { m_parent = parent; }

  const std::vector<std::unique_ptr<IElement>> &GetChildren() const override {
    return m_children;
  }

  void AddChild(std::unique_ptr<IElement> child) override {
    child->SetParent(this);
    m_children.push_back(std::move(child));
  }

private:
  ui::styleConfig m_style;
  ui::LayoutAccumulation m_geometry;
  IElement *m_parent = nullptr;
  std::vector<std::unique_ptr<IElement>> m_children;
};
} // namespace ui
