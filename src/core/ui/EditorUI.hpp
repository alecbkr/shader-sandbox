#pragma once

#include <core/input/ContextManager.hpp>

#include "components/SearchText.hpp"
#include "core/EditorEngine.hpp"

class EditorEngine;
class Logger;

class EditorUI {
public:
    EditorUI();
    void render();
    bool initialize(Logger* _loggerPtr, EditorEngine* _editorEngPtr, ContextManager* _contextManagerPtr);
    static void drawActiveFind(std::string activeLine, ImVec2 textPos);
    static SearchText searcher;
private:
    void renderEditor(Editor* editor);
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EditorEngine* editorEngPtr = nullptr;
    ContextManager* contextManagerPtr = nullptr;
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);
    bool findBar;
};
