#pragma once
#include <string>
#include <vector>

#include "EventTypes.hpp"
#include "imgui.h"
//#include "ui/EditorUI.hpp"
#include "ui/TextEditor.h"

class Logger;
class EventDispatcher;
class ModelCache;
class ShaderRegistry;

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
    EditorEngine();
    std::vector<Editor*> editors;
    int activeEditor;
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, ShaderRegistry* _shaderRegPtr);
    void shutdown();
    void createFile(const std::string& filePath);
    std::string findNextUntitledNumber();
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    bool spawnEditor(const EventPayload& payload);
    bool renameEditor(const EventPayload& payload);
    bool deleteEditor(const EventPayload& payload); 
};
