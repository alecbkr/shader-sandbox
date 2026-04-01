#pragma once 

#include "core/ui/modals/IModal.hpp"
#include "object/ModelCache.hpp"
#include "imgui.h"
#include <string> 
#include <unordered_set>
#include <filesystem>


class InspectorEngine;
class EventDispatcher;  
struct Project; 

static const std::unordered_set<std::string> supportedModelExtensions = {
    ".obj", ".gltf", ".glb", ".fbx"
}; 

enum class AddObjectPage {
    PRESET_ASSETS, 
    IMPORTED_ASSETS, 
}; 

class AddObjectModal final : public IModal{
    public: 
    AddObjectModal() = default; 
    bool initialize(ModelCache* _modelCachePtr, InspectorEngine* _inspectorEngPtr, Project* _projectPtr, EventDispatcher* _eventsPtr); 
    static constexpr const char* ID = "Add Object";
    std::string_view id() const override { return ID; }
    void draw() override; 

    private:
    struct PresetItem {
        const char* name; 
        ModelType type; 
    }; 
    
    ModelCache* modelCachePtr = nullptr; 
    InspectorEngine* inspectorEngPtr = nullptr; 
    Project* projectPtr = nullptr; 
    EventDispatcher* eventsPtr = nullptr; 

    bool initialized = false; 
    AddObjectPage page = AddObjectPage::PRESET_ASSETS; 

    // components
    void drawHeader(); 
    void drawPresetModelPage(); 
    void drawImportedModelPage(); 

    // helpers 
    void drawDirectoryNode(const std::filesystem::path& dirPath); 
    bool isValidFileExtension(const std::filesystem::directory_entry &entry); 
    void drawAssetTableRow(const std::string& name, const std::string& type, std::function<void()> onClick);
};