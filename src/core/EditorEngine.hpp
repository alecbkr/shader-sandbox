#pragma once
#include <string>
#include <vector>

#include "EventTypes.hpp"
#include "imgui.h"
#include "ui/EditorUI.hpp"

struct Editor {
    Editor(unsigned int bufferSize, std::string filePath, std::string fileName);
    char* inputTextBuffer;
    unsigned int bufferSize;
    int previousTextLen;
    int lineCount;
    std::string filePath;
    std::string fileName;
    void destroy();
};

class EditorEngine {
public:
    static std::vector<Editor*> editors;
    static int activeEditor;
    static bool initialize();
    static std::string getFileContents(std::string filename);
    static void createFile(const std::string& filePath);
    static int EditorInputCallback(ImGuiInputTextCallbackData* data);
private:
    static bool spawnEditor(const EventPayload& payload);
    static bool renameEditor(const EventPayload& payload);
    static bool deleteEditor(const EventPayload& payload);
    static void matchBrace(ImGuiInputTextCallbackData* data, Editor* editor);
    static void updateLineCount(ImGuiInputTextCallbackData* data, Editor* editor);
    static void updatePropertiesDueToMassDelete(ImGuiInputTextCallbackData* data, Editor* editor);
    static void updatePropertiesDueToMassInsert(ImGuiInputTextCallbackData* data, Editor* editor);
};
