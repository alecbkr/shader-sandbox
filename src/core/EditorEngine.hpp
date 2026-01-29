#pragma once
#include <string>
#include <vector>

#include "EventTypes.hpp"
#include "imgui.h"
#include "ui/EditorUI.hpp"
#include "ui/TextEditor.h"

struct Editor {
    TextEditor textEditor;
    Editor(std::string filePath, std::string fileName, unsigned int modelID);
    std::string filePath;
    std::string fileName;
    unsigned int modelID;
    void destroy();
};

class EditorEngine {
public:
    static std::vector<Editor*> editors;
    static int activeEditor;
    static bool initialize();
    static std::string getFileContents(std::string filename);
    static void createFile(const std::string& filePath);
    static std::string findNextUntitledNumber();
private:
    static bool spawnEditor(const EventPayload& payload);
    static bool renameEditor(const EventPayload& payload);
    static bool deleteEditor(const EventPayload& payload);
    
};
