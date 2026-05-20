#include "layout/elements/div.hpp"
#include "windowing/interface.hpp"
#include "windowing/linux/window_sdl.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

int main() {
  try {
    ui::WindowConfig config{"Atomic Flow Engine Test", 1200, 720, 100, 100};
    auto window = ui::SDLWindow::Create(config);

    auto rootDiv = std::make_unique<ui::DivElement>();
    ui::styleConfig rootStyle{};
    rootStyle.pos = {20.0f, 20.0f};
    rootStyle.size = {1160.0f, 680.0f};
    rootStyle.color = {0.11f, 0.12f, 0.13f, 1.0f};

    rootStyle.flexDirection = ui::FlexDirection::Row;
    rootStyle.padding = {30.0f, 30.0f, 30.0f, 30.0f};
    rootStyle.gap = {25.0f, 0.0f};
    rootDiv->GetStyle() = rootStyle;

    auto leftBox = std::make_unique<ui::DivElement>();
    ui::styleConfig leftStyle{};
    leftStyle.size = {350.0f, 620.0f};
    leftStyle.color = {0.18f, 0.48f, 0.90f, 1.0f}; // Cobalt Blue
    leftBox->GetStyle() = leftStyle;

    auto autoGrowingBox = std::make_unique<ui::DivElement>();
    ui::styleConfig growStyle{};
    growStyle.size = {400.0f, -1.0f};
    growStyle.color = {0.22f, 0.24f, 0.26f, 1.0f};
    growStyle.padding = {20.0f, 20.0f, 20.0f, 20.0f};
    growStyle.gap = {0.0f, 15.0f};
    growStyle.flexDirection = ui::FlexDirection::Column;
    autoGrowingBox->GetStyle() = growStyle;

    auto innerBoxA = std::make_unique<ui::DivElement>();
    ui::styleConfig innerA{};
    innerA.size = {360.0f, 100.0f};
    innerA.color = {0.85f, 0.35f, 0.25f, 1.0f};
    innerBoxA->GetStyle() = innerA;

    auto innerBoxB = std::make_unique<ui::DivElement>();
    ui::styleConfig innerB{};
    innerB.size = {360.0f, 150.0f};
    innerB.color = {0.30f, 0.65f, 0.40f, 1.0f};
    innerBoxB->GetStyle() = innerB;

    autoGrowingBox->AddChild(std::move(innerBoxA));
    autoGrowingBox->AddChild(std::move(innerBoxB));

    rootDiv->AddChild(std::move(leftBox));
    rootDiv->AddChild(std::move(autoGrowingBox));

    window->SetRootNode(std::move(rootDiv));

    while (!window->should_close()) {
      window->poll_events();
      window->render();
    }
  } catch (const std::exception &e) {
    std::cerr << "Engine Crash: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
