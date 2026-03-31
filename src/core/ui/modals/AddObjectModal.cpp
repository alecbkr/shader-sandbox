#include "AddObjectModal.hpp"
#include "core/InspectorEngine.hpp"
#include "application/Project.hpp"
#include "core/EventDispatcher.hpp"
#include <filesystem>

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
    if (ImGui::BeginChild("ModalRootSize", ImVec2(600, 400), false)) { 
        
        drawHeader();

        // Content Area
        if (ImGui::BeginChild("##AddObjectContent", ImVec2(0, 0), false)) {
            if (page == AddObjectPage::PRESET_ASSETS) drawPresetModelPage();
            else drawImportedModelPage();
        }
        ImGui::EndChild();
        ImGui::EndChild(); 
    }

    // Footer
    ImGui::Separator();
    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
}

void AddObjectModal::drawHeader() {
    float availWidth = ImGui::GetContentRegionAvail().x;

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));   
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));
    ImVec2 tabSize(availWidth * 0.5f, 0.0f); 
    
    // Preset Assets Tab
    if (page == AddObjectPage::PRESET_ASSETS) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    else ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
    if (ImGui::Button("Preset Objects", tabSize)) page = AddObjectPage::PRESET_ASSETS;
    ImGui::PopStyleColor();
    
    ImGui::SameLine(0, 0); 
    
    // Imported Assets Tab
    if (page == AddObjectPage::IMPORTED_ASSETS) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    else ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
    if (ImGui::Button("Imported Objects", tabSize)) page = AddObjectPage::IMPORTED_ASSETS;
    ImGui::PopStyleColor(); 

    ImGui::PopStyleVar(2);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void AddObjectModal::drawPresetModelPage() {
    ImGui::Spacing(); 

    ImGui::BeginChild("##PresetObjectsList", ImVec2(0, 0), true);
    
    PresetItem presets[] = {
        {"Plane", ModelType::PlanePreset},
        {"Pyramid", ModelType::PyramidPreset},
        {"Cube", ModelType::CubePreset}
    };

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingStretchProp; 

    if (ImGui::BeginTable("##PresetAssetsTable", 2, flags)) {
        
        // table headers
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers); 
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Asset Name");        
        ImGui::TableNextColumn();
        ImGui::TextDisabled("Type");

        for (const auto&preset : presets) {
            std::string label = std::string("Add ") + preset.name; 
            drawAssetTableRow(label, "Preset", [this, preset] () {
                modelCachePtr->createPreset(preset.type); 
                inspectorEngPtr->refreshUniforms(); 
                ImGui::CloseCurrentPopup(); 
            }); 
        }
        ImGui::EndTable(); 
    }
    ImGui::EndChild();
}

void AddObjectModal::drawImportedModelPage() {
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

        // check to see if there are any valid 3d files we can parse through the assets path 
        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
            if (entry.is_regular_file()) {
                if(isValidFileExtension(entry)) {
                    isFoundModels = true; 
                    break; 
                }
            }
        }
    
        if (!isFoundModels) {
            ImGui::TextDisabled("No 3D models (.obj, .gltf, .glb, .fbx) found in the project assets folder.");
        } else {
            ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY;
            if (ImGui::BeginTable("##ImportedAssetsTable", 2, flags)) {
                
                // table headers
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers); 
                ImGui::TableNextColumn();
                ImGui::TextDisabled("Asset Name");        
                ImGui::TableNextColumn();
                ImGui::TextDisabled("Type");

                drawDirectoryNode(assetsPath); 
                ImGui::EndTable(); 
            }
        }
    }
    ImGui::EndChild();
}

// replicates the assets tab folder structure inside the modal 
void AddObjectModal::drawDirectoryNode(const std::filesystem::path& dirPath) {
    std::vector<std::filesystem::path> directories; 
    std::vector<std::filesystem::path> validFiles; 

    // grab the corresponding object files from the imported assets directory
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            directories.push_back(entry.path()); 
        } else if (entry.is_regular_file()) {
            if (isValidFileExtension(entry)) validFiles.push_back(entry.path()); 
        }
    }

    // draw the folders/dropdowns 
    for (const auto& subdir : directories) {
        ImGui::TableNextRow(); 
        ImGui::TableNextColumn(); 

        std::string folderName = subdir.filename().string(); 
        bool isNodeOpen = ImGui::TreeNodeEx(folderName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth); 

        ImGui::TableNextColumn(); 
        ImGui::TextDisabled("Folder"); 

        if (isNodeOpen) {
            drawDirectoryNode(subdir); 
            ImGui::TreePop();
        }
    
    }
        
    for (const auto& file : validFiles) {
        std::string filename = file.filename().string(); 
        std::string fileExt = file.extension().string(); 

        if (!fileExt.empty() && fileExt[0] == '.') fileExt = fileExt.substr(1); 
        std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::toupper); 
        
        std::string filePath = file.string(); 

        drawAssetTableRow(filename, fileExt, [this, filePath](){
            eventsPtr->TriggerEvent(Event{
                EventType::LoadModel, false, LoadModelPayload{filePath}
            }); 
            ImGui::CloseCurrentPopup(); 
        });
    }
}

bool AddObjectModal::isValidFileExtension(const std::filesystem::directory_entry &entry) {
    std::string ext = entry.path().extension().string(); 
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (supportedModelExtensions.contains(ext)) return true; 
    
    return false; 
}

void AddObjectModal::drawAssetTableRow(const std::string& name, const std::string& fileExt, std::function<void()> onClick) {
    ImGui::TableNextRow(); 
    ImGui::TableNextColumn(); 
    ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns;

    // col 1
    if (ImGui::Selectable(name.c_str(), false, flags)) onClick(); 

    // col 2
    ImGui::TableNextColumn(); 
    ImGui::TextDisabled("%s", fileExt.c_str()); 
}