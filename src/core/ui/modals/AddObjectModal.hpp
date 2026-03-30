#pragma once 

#include "core/ui/modals/IModal.hpp"
#include <string> 

class ModelCache; 
class InspectorEngine; 

enum class AddObjectPage {
    PRESET_ASSETS, 
    CUSTOM_ASSETS, 
}; 

class AddObjectModal final : public IModal{
    public: 
    AddObjectModal() = default; 
    bool initialize(ModelCache* _modelCachePtr, InspectorEngine* _inspectorEngPtr); 
    static constexpr const char* ID = "Add Object Modal";
    std::string_view id() const override { return ID; }
    void draw() override; 

    private: 
    ModelCache* modelCachePtr; 
    InspectorEngine* inspectorEngPtr; 
    char customModelPath[256] = ""; 

    bool initialized = false; 
    AddObjectPage page = AddObjectPage::PRESET_ASSETS; 

    void drawPresetModelPage(); 
    void drawCustomModelPage(); 
};