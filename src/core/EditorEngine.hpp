#pragma once
#include <string>
#include <vector>

#include "EventTypes.hpp"
#include "imgui.h"
#include "ui/EditorUI.hpp"

struct Editor {
    Editor(unsigned int _bufferSize);
    char* inputTextBuffer;
    unsigned int bufferSize;
    int previousTextLen;
    int lineCount;
    void destroy();
};

class EditorEngine {
public:
    static std::vector<Editor*> editors;
    static bool initialize();
    static void spawnEditor(unsigned int bufferSize);
    static std::string getFileContents(const char *filename);
    static int EditorInputCallback(ImGuiInputTextCallbackData* data);
private:
    static void matchBrace(ImGuiInputTextCallbackData* data, Editor* editor);
    static void updateLineCount(ImGuiInputTextCallbackData* data, Editor* editor);
    static void updatePropertiesDueToMassDelete(ImGuiInputTextCallbackData* data, Editor* editor);
    static void updatePropertiesDueToMassInsert(ImGuiInputTextCallbackData* data, Editor* editor);
};
