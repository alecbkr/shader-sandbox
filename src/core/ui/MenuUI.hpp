#pragma once

#include "imgui/imgui.h"
#include "core/MenuEngine.hpp"

class MenuUI {
public:
    MenuUI();
    void render();

private:
    void drawMenuBar();
    void drawMenuItem(const MenuItem& item);
};
