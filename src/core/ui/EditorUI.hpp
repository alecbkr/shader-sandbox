#pragma once

#include "components/SearchText.hpp"
#include "core/EditorEngine.hpp"

class EditorUI {
public:
    static void render();
    static bool initialize();
    static void drawActiveFind(std::string activeLine, ImVec2 textPos);
    static SearchText searcher;

private:
    static float targetWidth;
    static float targetHeight;
    static ImVec2 windowPos;
    static bool findBar;
};
