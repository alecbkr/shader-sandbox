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

    // Top Navigation Toggle
    if (ImGui::BeginChild("ModalRootSize", ImVec2(500, 400), false)) { 
        ImGui::SetNextItemWidth(-1); 

        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));   
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));

        ImVec2 toggleSize(ImGui::GetContentRegionAvail().x * 0.5f, 40); 
        
        // Preset Assets Button
        if (page == AddObjectPage::PRESET_ASSETS) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        else ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        
        if (ImGui::Button("Preset Assets", toggleSize)) page = AddObjectPage::PRESET_ASSETS;
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 0); // remove spacing in-between the buttons 
        
        // Custom Assets Button
        if (page == AddObjectPage::CUSTOM_ASSETS) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        else ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        
        if (ImGui::Button("Custom Assets", toggleSize)) page = AddObjectPage::CUSTOM_ASSETS;
        ImGui::PopStyleColor(); 
        ImGui::PopStyleVar(2);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Content Area
        if (ImGui::BeginChild("##AddObjectContent", ImVec2(0, 0), false)) {
            if (page == AddObjectPage::PRESET_ASSETS) {
                drawPresetModelPage();
            } else {
                drawCustomModelPage();
            }
            ImGui::EndChild();
        }

        ImGui::EndChild(); 
    }

    // Footer
    ImGui::Separator();
    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
}

void AddObjectModal::drawPresetModelPage() {
    ImGui::Text("Preset Models"); 
    ImGui::Separator(); 
    ImGui::Spacing(); 

    ImVec2 btnSize(-1.0f, 0.0f); 

    if (ImGui::Button("Add Plane", btnSize)) {
        modelCachePtr->createPreset(ModelType::PlanePreset);
        inspectorEngPtr->refreshUniforms();
        ImGui::CloseCurrentPopup(); 
    }

    ImGui::Spacing();

    if (ImGui::Button("Add Pyramid", btnSize)) {
        modelCachePtr->createPreset(ModelType::PyramidPreset);
        inspectorEngPtr->refreshUniforms();
        ImGui::CloseCurrentPopup();
    }
    ImGui::Spacing();
    if (ImGui::Button("Add Cube", btnSize)) {
        modelCachePtr->createPreset(ModelType::CubePreset);
        inspectorEngPtr->refreshUniforms();
        ImGui::CloseCurrentPopup();
    }
}

void AddObjectModal::drawCustomModelPage() {

}