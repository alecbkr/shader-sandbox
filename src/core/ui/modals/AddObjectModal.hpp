#pragma once 

#include "core/ui/modals/IModal.hpp"
#include <string> 

class ModelCache; 
class InspectorEngine;
class EventDispatcher;  
struct Project; 

enum class AddObjectPage {
    PRESET_ASSETS, 
    IMPORTED_ASSETS, 
}; 

class AddObjectModal final : public IModal{
    public: 
    AddObjectModal() = default; 
    bool initialize(ModelCache* _modelCachePtr, InspectorEngine* _inspectorEngPtr, Project* _projectPtr, EventDispatcher* _eventsPtr); 
    static constexpr const char* ID = "Add Object Modal";
    std::string_view id() const override { return ID; }
    void draw() override; 

    private: 
    ModelCache* modelCachePtr = nullptr; 
    InspectorEngine* inspectorEngPtr = nullptr; 
    Project* projectPtr = nullptr; 
    EventDispatcher* eventsPtr = nullptr; 

    char customModelPath[256] = ""; 

    bool initialized = false; 
    AddObjectPage page = AddObjectPage::PRESET_ASSETS; 

    void drawPresetModelPage(); 
    void drawImportedModelPage(); 
};