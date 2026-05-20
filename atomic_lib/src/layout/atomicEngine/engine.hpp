#pragma once
#include "layout/element.hpp"
#include "math/vec.hpp"
#include "renderer/style.hpp"
#include <algorithm>
#include <vector>

namespace ui {

class AtomicEngine {
public:
  AtomicEngine() = default;
  ~AtomicEngine() = default;

  void ProcessLayout(IElement *rootNode,
                     const math::vec2<float> &canvasBounds) {
    if (!rootNode)
      return;

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
  void ExecSizingPass(IElement *node,
                      const math::vec2<float> &parentAllocation) {
    const ui::styleConfig &style = node->GetStyle();
    LayoutAccumulation &metrics = node->GetLayoutMetrics();

    math::vec2<float> innerContentAllocation = {
        std::max(0.0f,
                 (style.size.x > 0.0f ? style.size.x : parentAllocation.x) -
                     (style.padding.w + style.padding.y)),
        std::max(0.0f,
                 (style.size.y > 0.0f ? style.size.y : parentAllocation.y) -
                     (style.padding.x + style.padding.z))};

    for (const auto &child : node->GetChildren()) {
      ExecSizingPass(child.get(), innerContentAllocation);
    }

    math::vec2<float> contentSize{0.0f, 0.0f};
    bool isRow = (style.flexDirection == FlexDirection::Row);
    size_t childCount = node->GetChildren().size();
    size_t activeGaps = (childCount > 1) ? (childCount - 1) : 0;

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

    if (isRow) {
      contentSize.x += (activeGaps * style.gap.x);
    } else {
      contentSize.y += (activeGaps * style.gap.y);
    }

    contentSize.x += (style.padding.w + style.padding.y);
    contentSize.y += (style.padding.x + style.padding.z);

    metrics.computed_size.x =
        (style.size.x >= 0.0f) ? style.size.x : contentSize.x;
    metrics.computed_size.y =
        (style.size.y >= 0.0f) ? style.size.y : contentSize.y;
  }

  void ExecPositionPass(IElement *node, math::vec2<float> &globalOrigin) {
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

      // Propagate the calculated position downwards
      ExecPositionPass(child.get(), metrics.global_position);
    }
  }

  void FlattenHierarchy(IElement *node) {
    m_linearizedRenderCache.push_back(node);
    for (const auto &child : node->GetChildren()) {
      FlattenHierarchy(child.get());
    }
  }

  std::vector<IElement *> m_linearizedRenderCache;
};

} // namespace ui
