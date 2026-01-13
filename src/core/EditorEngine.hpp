#pragma once
#include <string>
#include <vector>

#include "EventTypes.hpp"
#include "imgui.h"
#include "ui/EditorUI.hpp"

class EditorEngine {
public:
    static std::vector<EditorUI*> editors;
    static bool spawnEditor(const EventPayload& payload);
    static std::string getFileContents(std::string filename);
    static int EditorInputCallback(ImGuiInputTextCallbackData* data);
private:
    static void matchBrace(ImGuiInputTextCallbackData* data, EditorUI* ui);
    static void updateLineCount(ImGuiInputTextCallbackData* data, EditorUI* ui);
    static void updatePropertiesDueToMassDelete(ImGuiInputTextCallbackData* data, EditorUI* ui);
    static void updatePropertiesDueToMassInsert(ImGuiInputTextCallbackData* data, EditorUI* ui);
};
