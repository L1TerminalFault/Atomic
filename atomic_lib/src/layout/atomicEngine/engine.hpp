#pragma once
#include "layout/element.hpp"
#include "layout/elements/text.hpp"
#include "math/vec.hpp"
#include "renderer/style.hpp"
#include <algorithm>
#include <variant>
#include <vector>

namespace ui {

class AtomicEngine {
public:
  AtomicEngine() = default;
  ~AtomicEngine() = default;

  void ProcessLayout(IElement *rootNode, const math::vec2<float> &canvasBounds,
                     ui::font::Font *defaultFont) {
    if (!rootNode)
      return;

    m_contextDefaultFont = defaultFont;
    ExecSizingPass(rootNode, canvasBounds);

    math::vec2<float> rootStartingPosition = rootNode->GetStyle().pos;
    ExecPositionPass(rootNode, rootStartingPosition);

    m_linearizedRenderCache.clear();
    FlattenHierarchy(rootNode);
  }

  const std::vector<IElement *> &GetRenderCache() const {
    return m_linearizedRenderCache;
  }

private:
  ui::font::Font *m_contextDefaultFont;
  void ExecSizingPass(IElement *node,
                      const math::vec2<float> &parentAllocation) {
    if (!node)
      return;

    const ui::styleConfig &style = node->GetStyle();
    LayoutAccumulation &metrics = node->GetLayoutMetrics();

    if (node->GetType() == ElementType::TEXT) {
      auto *textNode = static_cast<ui::TextElement *>(node);

      // Grab the active font handle currently cached inside the global UI
      // Context
      ui::font::Font *activeFont =
          style.font ? reinterpret_cast<ui::font::Font *>(style.font)
                     : m_contextDefaultFont;

      // Compute string layout dimensions directly from FreeType metrics bounds
      math::vec2<float> intrinsicSize =
          textNode->ComputeIntrinsicBounds(activeFont);

      if (std::holds_alternative<ui::SizeFit>(style.size.x)) {
        metrics.computed_size.x = intrinsicSize.x;
      } else if (std::holds_alternative<float>(style.size.x)) {
        metrics.computed_size.x = std::get<float>(style.size.x);
      }

      if (std::holds_alternative<ui::SizeFit>(style.size.y)) {
        metrics.computed_size.y = intrinsicSize.y;
      } else if (std::holds_alternative<float>(style.size.y)) {
        metrics.computed_size.y = std::get<float>(style.size.y);
      }

      return;
    }

    bool isRow = (style.flexDirection == FlexDirection::Row);
    size_t childCount = node->GetChildren().size();
    size_t activeGaps = (childCount > 1) ? (childCount - 1) : 0;

    // Calculate current padding bounds up front to bound internal budgets
    // safely
    float paddingX = style.padding.w + style.padding.y; // Left + Right
    float paddingY = style.padding.x + style.padding.z; // Top + Bottom

    float currentBoundaryX = std::holds_alternative<float>(style.size.x)
                                 ? std::get<float>(style.size.x)
                                 : parentAllocation.x;
    float currentBoundaryY = std::holds_alternative<float>(style.size.y)
                                 ? std::get<float>(style.size.y)
                                 : parentAllocation.y;

    math::vec2<float> currentInnerCapacity = {
        std::max(0.0f, currentBoundaryX - paddingX),
        std::max(0.0f, currentBoundaryY - paddingY)};
    // PASS 1: Calculate Total Fixed Main Budgets & Count Fill Elements
    float totalFixedMainAxis = 0.0f;
    size_t fillCountMainAxis = 0;

    for (const auto &child : node->GetChildren()) {
      const auto &childStyle = child->GetStyle();
      const auto &mainSize = isRow ? childStyle.size.x : childStyle.size.y;

      if (std::holds_alternative<float>(mainSize)) {
        float marginTotal = isRow ? (childStyle.margin.w + childStyle.margin.y)
                                  : (childStyle.margin.x + childStyle.margin.z);
        totalFixedMainAxis += std::get<float>(mainSize) + marginTotal;
      } else if (std::holds_alternative<ui::SizeFill>(mainSize)) {
        fillCountMainAxis++;
      }
    }

    // PASS 2: Calculate Main Axis Remainder and Cap Slices Safely
    float gapsMain = activeGaps * (isRow ? style.gap.x : style.gap.y);
    float innerMainCapacity =
        isRow ? currentInnerCapacity.x : currentInnerCapacity.y;

    float rawRemainingSpace = innerMainCapacity - totalFixedMainAxis - gapsMain;
    float availableSpaceForFill =
        std::max(0.0f, rawRemainingSpace); // capped to min of 0.0

    float fillSlice = (fillCountMainAxis > 0)
                          ? (availableSpaceForFill / fillCountMainAxis)
                          : 0.0f;

    // PASS 3: Deep Constrained Recursion Pass
    for (const auto &child : node->GetChildren()) {
      const auto &childStyle = child->GetStyle();

      math::vec2<float> childAllocationBudget;

      // Resolve Width Constraint (X-Axis)
      if (std::holds_alternative<float>(childStyle.size.x)) {
        childAllocationBudget.x = std::get<float>(childStyle.size.x);
      } else if (std::holds_alternative<ui::SizeFill>(childStyle.size.x)) {
        childAllocationBudget.x = isRow ? fillSlice : currentInnerCapacity.x;
      } else { // ui::SizeFit
        childAllocationBudget.x = currentInnerCapacity.x;
      }

      // Resolve Height Constraint (Y-Axis)
      if (std::holds_alternative<float>(childStyle.size.y)) {
        childAllocationBudget.y = std::get<float>(childStyle.size.y);
      } else if (std::holds_alternative<ui::SizeFill>(childStyle.size.y)) {
        childAllocationBudget.y = !isRow ? fillSlice : currentInnerCapacity.y;
      } else { // ui::SizeFit
        childAllocationBudget.y = currentInnerCapacity.y;
      }

      // Enforce hard boundaries: elements can never receive a budget exceeding
      // inner bounds
      childAllocationBudget.x =
          std::min(childAllocationBudget.x, currentInnerCapacity.x);
      childAllocationBudget.y =
          std::min(childAllocationBudget.y, currentInnerCapacity.y);

      ExecSizingPass(child.get(), childAllocationBudget);
    }

    // PASS 4: Accumulate Child Bounds (Excluding Parent Padding)
    math::vec2<float> contentSize{0.0f, 0.0f};

    for (size_t i = 0; i < childCount; ++i) {
      IElement *child = node->GetChildren()[i].get();
      const auto &childMetrics = child->GetLayoutMetrics();
      const auto &childStyle = child->GetStyle();

      float childWidthWithMargin = childMetrics.computed_size.x +
                                   childStyle.margin.w + childStyle.margin.y;
      float childHeightWithMargin = childMetrics.computed_size.y +
                                    childStyle.margin.x + childStyle.margin.z;

      if (isRow) {
        contentSize.x += childWidthWithMargin;
        contentSize.y = std::max(contentSize.y, childHeightWithMargin);
      } else { // FlexDirection::Column
        contentSize.y += childHeightWithMargin;
        contentSize.x = std::max(contentSize.x, childWidthWithMargin);
      }
    }

    if (childCount > 0) {
      if (isRow)
        contentSize.x += gapsMain;
      else
        contentSize.y += gapsMain;
    }

    // PASS 5: Resolve Node Final Sizing Metrics Outward
    auto resolveFinalMetric = [](const ui::Size &sizeVariant, float contentVal,
                                 float paddingTotal,
                                 float parentAlloc) -> float {
      if (std::holds_alternative<float>(sizeVariant)) {
        return std::get<float>(sizeVariant);
      }
      if (std::holds_alternative<ui::SizeFit>(sizeVariant)) {
        // Content size already includes gaps and margins, add current node
        // padding now
        return contentVal + paddingTotal;
      }
      // SizeFill items are strictly clamped to the assigned parent allocation
      // limit
      return parentAlloc;
    };

    metrics.computed_size.x = resolveFinalMetric(style.size.x, contentSize.x,
                                                 paddingX, parentAllocation.x);
    metrics.computed_size.y = resolveFinalMetric(style.size.y, contentSize.y,
                                                 paddingY, parentAllocation.y);
  }

  void ExecPositionPass(IElement *node, const math::vec2<float> &globalOrigin) {
    if (!node)
      return;

    LayoutAccumulation &metrics = node->GetLayoutMetrics();
    const ui::styleConfig &style = node->GetStyle();

    metrics.global_position = globalOrigin + metrics.local_position;

    math::vec2<float> childCursorOffset{style.padding.w, style.padding.x};
    bool isRow = (style.flexDirection == FlexDirection::Row);

    for (const auto &child : node->GetChildren()) {
      LayoutAccumulation &childMetrics = child->GetLayoutMetrics();
      const auto &childStyle = child->GetStyle();

      if (isRow) {
        childCursorOffset.x += childStyle.margin.w; // Add Left Margin
        childMetrics.local_position = {
            childCursorOffset.x, childCursorOffset.y + childStyle.margin.x};

        childCursorOffset.x +=
            childMetrics.computed_size.x + childStyle.margin.y + style.gap.x;
      } else {                                      // FlexDirection::Column
        childCursorOffset.y += childStyle.margin.x; // Add Top Margin
        childMetrics.local_position = {
            childCursorOffset.x + childStyle.margin.w, childCursorOffset.y};

        childCursorOffset.y +=
            childMetrics.computed_size.y + childStyle.margin.z + style.gap.y;
      }

      ExecPositionPass(child.get(), metrics.global_position);
    }
  }

  void FlattenHierarchy(IElement *node) {
    if (!node)
      return;
    m_linearizedRenderCache.push_back(node);
    for (const auto &child : node->GetChildren()) {
      FlattenHierarchy(child.get());
    }
  }

  std::vector<IElement *> m_linearizedRenderCache;
};

} // namespace ui
