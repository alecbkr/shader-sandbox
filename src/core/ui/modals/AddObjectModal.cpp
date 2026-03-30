#include "AddObjectModal.hpp"
#include "object/ModelCache.hpp"
#include "core/InspectorEngine.hpp"

bool AddObjectModal::initialize(ModelCache* _modelCachePtr, InspectorEngine* _inspectorEngPtr) {
    if (initialized) return false; 

    modelCachePtr = _modelCachePtr; 
    inspectorEngPtr = _inspectorEngPtr; 

    initialized = true; 
    return true; 
}

void AddObjectModal::draw() {
    if (ImGui::BeginTabBar("Add Objects")) {
        drawPresetModelTab(); 
        drawCustomModelTab(); 

        ImGui::EndTabBar();
    }

    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
    }
}

void AddObjectModal::drawPresetModelTab() {
    if (ImGui::BeginTabItem("Presets")){
        if (ImGui::Button("Add Plane")) {
            modelCachePtr->createPreset(ModelType::PlanePreset);
            inspectorEngPtr->refreshUniforms();
            ImGui::CloseCurrentPopup(); 
        }
        if (ImGui::Button("Add Pyramid")) {
            modelCachePtr->createPreset(ModelType::PyramidPreset);
            inspectorEngPtr->refreshUniforms();
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Add Cube")) {
            modelCachePtr->createPreset(ModelType::CubePreset);
            inspectorEngPtr->refreshUniforms();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndTabItem();
    }
}

void AddObjectModal::drawCustomModelTab() {

}