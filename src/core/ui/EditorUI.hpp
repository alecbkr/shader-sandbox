#pragma once
#include <filesystem>

#include "core/EditorEngine.hpp"

class EditorEngine;
class Logger;

class EditorUI {
public:
    EditorUI();
    void render();
    bool initialize(Logger* _loggerPtr, EditorEngine* _editorEngPtr);
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EditorEngine* editorEngPtr = nullptr;
    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);
};
