#pragma once
#include <filesystem>

#include "core/EditorEngine.hpp"

class EditorUI {
public:
    static void render();
    static bool initialize();
private:
    static float targetWidth;
    static float targetHeight;
    static ImVec2 windowPos;
};
