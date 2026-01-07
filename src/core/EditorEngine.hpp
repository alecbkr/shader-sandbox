#pragma once
#include <string>
#include <vector>

#include "imgui.h"
#include "ui/EditorUI.hpp"

class EditorEngine {
public:
    static std::vector<EditorUI*> editors;
    static void spawnEditor(unsigned int bufferSize);
    static std::string getFileContents(const char *filename);
    static int EditorInputCallback(ImGuiInputTextCallbackData* data);
private:
    static void matchBrace(ImGuiInputTextCallbackData* data, EditorUI* ui);
    static void updateLineCount(ImGuiInputTextCallbackData* data, EditorUI* ui);
    static void updatePropertiesDueToMassDelete(ImGuiInputTextCallbackData* data, EditorUI* ui);
    static void updatePropertiesDueToMassInsert(ImGuiInputTextCallbackData* data, EditorUI* ui);
};
