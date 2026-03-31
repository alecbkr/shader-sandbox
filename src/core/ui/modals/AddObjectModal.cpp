#include "AddObjectModal.hpp"
#include "object/ModelCache.hpp"
#include "core/InspectorEngine.hpp"
#include "application/Project.hpp"
#include "core/EventDispatcher.hpp"

bool AddObjectModal::initialize(ModelCache* _modelCachePtr, InspectorEngine* _inspectorEngPtr, Project* _projectPtr, EventDispatcher* _eventsPtr) {
    if (initialized) return false; 

    modelCachePtr = _modelCachePtr; 
    inspectorEngPtr = _inspectorEngPtr; 
    projectPtr = _projectPtr; 
    eventsPtr = _eventsPtr; 

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
        
        if (ImGui::Button("Preset Objects", toggleSize)) page = AddObjectPage::PRESET_ASSETS;
        ImGui::PopStyleColor();
        
        ImGui::SameLine(0, 0); // remove spacing in-between the buttons 
        
        // IMPORTED Assets Button
        if (page == AddObjectPage::IMPORTED_ASSETS) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        else ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        
        if (ImGui::Button("Imported Objects", toggleSize)) page = AddObjectPage::IMPORTED_ASSETS;
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
                drawImportedModelPage();
            }
        }
        ImGui::EndChild();
        ImGui::EndChild(); 
    }

    // Footer
    ImGui::Separator();
    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
}

void AddObjectModal::drawPresetModelPage() {
    ImGui::Text("Preset Objects"); 
    ImGui::Separator(); 
    ImGui::Spacing(); 

    if (ImGui::BeginChild("##PresetObjectsList", ImVec2(0, 0), true)) {
        if (ImGui::Selectable("Add Plane")) {
            modelCachePtr->createPreset(ModelType::PlanePreset);
            inspectorEngPtr->refreshUniforms();
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::Spacing();
        if (ImGui::Selectable("Add Pyramid")) {
            modelCachePtr->createPreset(ModelType::PyramidPreset);
            inspectorEngPtr->refreshUniforms();
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        if (ImGui::Selectable("Add Cube")) {
            modelCachePtr->createPreset(ModelType::CubePreset);
            inspectorEngPtr->refreshUniforms();
            ImGui::CloseCurrentPopup();
        }
    }
    ImGui::EndChild();
}

void AddObjectModal::drawImportedModelPage() {
    ImGui::TextUnformatted("Imported Objects"); 
    ImGui::Separator(); 
    ImGui::Spacing(); 

    if (ImGui::BeginChild("##ImportedObjectsList", ImVec2(0, 0), true)) { 

        if(!projectPtr) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Project data unavailable.");
            ImGui::EndChild();
            return;
        }

        std::filesystem::path assetsPath = projectPtr->projectRoot / "assets";

        if (!std::filesystem::exists(assetsPath)) {
            ImGui::TextDisabled("Assets directory not found"); 
            ImGui::EndChild(); 
            return; 
        }

        bool isFoundModels = false; 

        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                if (supportedModelExtensions.contains(ext)) {
                    isFoundModels = true;
                    
                    std::string displayPath = std::filesystem::relative(entry.path(), assetsPath).string();

                    if (ImGui::Selectable(displayPath.c_str())) {
                        eventsPtr->TriggerEvent(Event{
                            EventType::LoadModel,
                            false,
                            LoadModelPayload{entry.path().string()}
                        });
                        
                        ImGui::CloseCurrentPopup();
                    }
                
                }
            }
        }
    
        if (!isFoundModels) {
            ImGui::TextDisabled("No 3D models (.obj, .gltf, .glb, .fbx)\nfound in the project assets folder.");
        }
    }
    ImGui::EndChild();
}