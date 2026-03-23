#pragma once
#include <string>
#include <vector>
#include <types.hpp>

#include "EventTypes.hpp"
#include "application/Project.hpp"
#include "application/Project.hpp"
#include "ui/TextEditor.h"

class Logger;
class EventDispatcher;
class ModelCache;
class ShaderRegistry;
struct SettingsStyles;

struct Editor {
    TextEditor textEditor;
    SearchText searcher;
    Editor(std::string filePath, std::string fileName, unsigned int modelID, SettingsStyles* styles, bool readOnly);
    std::string filePath;
    std::string fileName;
    bool readOnly;
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
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, ShaderRegistry* _shaderRegPtr, SettingsStyles* styles, Project* project);
    void shutdown();
    void createFile(const std::string& filePath);
    std::string findNextFileNumber(const std::string& startingName);
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    SettingsStyles* stylesPtr = nullptr;
    Project* projectPtr = nullptr;
    bool spawnEditor(const EventPayload& payload);
    bool renameEditor(const EventPayload& payload);
    bool deleteEditor(const EventPayload& payload);
    bool cloneFile(const EventPayload& payload);
};
