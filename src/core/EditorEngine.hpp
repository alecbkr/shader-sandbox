#pragma once
#include <string>
#include <vector>
#include <types.hpp>

#include "EventTypes.hpp"
#include "imgui.h"
//#include "ui/EditorUI.hpp"
#include "ui/TextEditor.h"

class Logger;
class EventDispatcher;
class ModelCache;
class ShaderRegistry;
struct SettingsStyles;

struct Editor {
    TextEditor textEditor;
    Editor(std::string filePath, std::string fileName, unsigned int modelID, SettingsStyles* styles);
    std::string filePath;
    std::string fileName;
    unsigned int modelID;

    SettingsStyles* stylesPtr = nullptr;
    u32 seenPaletteVersion = 0;
    void render();
    void applyPaletteIfOutdated();
    void destroy();
};

class EditorEngine {
public:
    EditorEngine();
    std::vector<Editor*> editors;
    int activeEditor;
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, ShaderRegistry* _shaderRegPtr, SettingsStyles* styles);
    void shutdown();
    void createFile(const std::string& filePath);
    std::string findNextUntitledNumber();
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    SettingsStyles* stylesPtr = nullptr;
    bool spawnEditor(const EventPayload& payload);
    bool renameEditor(const EventPayload& payload);
    bool deleteEditor(const EventPayload& payload); 
};
