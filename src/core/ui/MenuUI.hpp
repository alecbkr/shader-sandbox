#pragma once

#include "imgui/imgui.h"
#include "core/MenuEngine.hpp"

class MenuUI {
public:
    static bool initialize();
    static void render();

private:
    static bool initialized;
    static void drawMenuBar();
    static void drawMenuItem(const MenuItem& item);
};
