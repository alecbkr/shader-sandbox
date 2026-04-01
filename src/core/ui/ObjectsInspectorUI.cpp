#include "core/ui/ObjectsInspectorUI.hpp"

#include "application/SettingsStyles.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/TextureRegistry.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "engine/ShaderProgram.hpp"
#include "imgui.h"
#include "object/Material.hpp"
#include "object/MaterialCache.hpp"
#include "object/Model.hpp"
#include "object/ModelCache.hpp"
#include "presets/PresetAssets.hpp"
#include "texture/Texture.hpp"
#include "core/ui/modals/ModalManager.hpp"
#include "core/ui/Fonts.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

ObjectsInspectorUI::ObjectsInspectorUI(SettingsStyles* styles) : styles(styles) {
    if (styles) {
        theme.bgColor = styles->assetsTreeBodyColor;
        // Derive a hover color from the base color so it always differs visibly.
        theme.bgColorHovered = ImVec4(
            theme.bgColor.x * 1.3f,
            theme.bgColor.y * 1.3f,
            theme.bgColor.z * 1.3f,
            theme.bgColor.w
        );
        theme.indentSize = styles->indentSpacing * 0.5f;
    }
}

void ObjectsInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, TextureRegistry* textureRegPtr, ModelCache* modelCachePtr, MaterialCache* materialCachePtr, Fonts* fonts, ModalManager* modalManager) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->inspectorTabBackgroundColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);

    float window_padding = styles->inspectorBodyPadding;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));

    if (ImGui::BeginChild("ObjectsContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        ImGui::PushFont(fonts->getL4());

        float inner_padding = styles->inspectorTitleInnerPadding;
        float directory_height = window_padding * 2 + inner_padding * 2 + ImGui::CalcTextSize("Materials").y;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0,0,0,0));

        if (ImGui::BeginChild("ObjectsTitle", ImVec2(0, directory_height), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->inspectorTitleBackgroundColor),
                styles->inspectorBodyRounding,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->inspectorBorderColor),
                styles->inspectorBodyRounding,
                ImDrawFlags_RoundCornersTop,
                styles->inspectorBorderThickness
            );

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(styles->inspectorTitleOffset, 0.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted("Objects");
            
            ImVec2 plusSize = ImGui::CalcTextSize("+");
            float buttonPaddingY = 0.0f;
            float buttonPaddingX = 6.0f;
            
            float buttonWidth = buttonPaddingX * 2 + plusSize.x;
            
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - buttonWidth);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(buttonPaddingX, buttonPaddingY));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
            if (ImGui::Button("+", ImVec2(buttonPaddingX * 2 + plusSize.x, buttonPaddingY * 2 + plusSize.y))) {
                if (modalManager) modalManager->open("Add Object");
            }
            ImGui::PopStyleVar(2);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopFont();

        if (ImGui::BeginChild("ObjectsContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->inspectorBodyColor),
                styles->inspectorBodyRounding,
                ImDrawFlags_RoundCornersBottom
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->inspectorBorderColor),
                styles->inspectorBodyRounding,
                ImDrawFlags_RoundCornersBottom,
                styles->inspectorBorderThickness
            );

            int i = 0;
            if (modelCachePtr->getAllModels().empty()) {
                ImGui::TextUnformatted("No Objects");
            }
            for (auto& model : modelCachePtr->getAllModels()) {
                auto& matIDReferences = model->getAllMaterialReferences();
                unsigned int modelID = model->ID;
                for (auto& [matID, matRefCount] : matIDReferences) {
                    if (!materialShaderMenus.contains(matID)) {
                        materialShaderMenus[matID] = MaterialShaderMenu{
                            .matID = matID,
                            .selection = 0,
                            .initialized = false,
                        };
                    }
                }
        
                if (!modelTextureMenus.contains(modelID)) {
                    modelTextureMenus[modelID] = ModelTextureMenu{
                        .modelID = modelID,
                        .uniformName = "baseTex",
                        .textureSelection = 0,
                        .unitSelection = 0,
                        .initialized = false,
                    };
                }
                //ModelTextureMenu& textureMenu = modelTextureMenus[modelID];
        
                // std::string label = "model " + std::to_string(modelID);
                // ImGui::PushID(label.c_str());
                ImGui::PushID(modelID); 
                ImGui::PushStyleColor(ImGuiCol_ChildBg, theme.bgColor); // Blue-ish background
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);   // <-- rounding radius
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f); // optional
                ImGui::PushStyleColor(ImGuiCol_Header,        theme.bgColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, theme.bgColorHovered);
                ImGui::PushStyleColor(ImGuiCol_HeaderActive,  theme.bgColor);
                ImGui::BeginChild("Container##", ImVec2(0, 0),  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

                if (drawModelHeader(model, modelCachePtr)) {
                    ImGui::Separator();
                    ImGui::Indent(theme.indentSize);
        
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                        ImGui::Indent(theme.indentSize);
                        drawModelPositionInput(*model);
                        ImGui::Separator();
                        drawModelScaleInput(*model);
                        ImGui::Separator();
                        drawModelOrientationInput(*model);
                        ImGui::Unindent(theme.indentSize);
                    }
                    // ALECS ATTEMPT
                    drawMeshesMenu(model, materialCachePtr, modelCachePtr, loggerPtr);
                    drawInstancesMenu(model, modelCachePtr, loggerPtr);
                        
                    // if (ImGui::CollapsingHeader("Material")) {
                    //     ImGui::Indent(theme.indentSize);
                    //     std::vector<const char*> shaderChoices;
                    //     shaderChoices.reserve(shaderRegPtr->getNumberOfPrograms());
                    //     const auto& shaders = shaderRegPtr->getPrograms();
                    //     for (auto& [name, shader] : shaders) {
                    //         shaderChoices.push_back(name.c_str());
                    //     }
                    //     for (auto [matID, matRefCount] : matIDReferences) {
                    //         MaterialShaderMenu shaderMenu{
                    //             .matID = matID
                    //         };
                    //         if (!shaderMenu.initialized) {
                    //             initializeMenu(shaderMenu, shaderChoices, loggerPtr, shaderRegPtr, materialCachePtr);
                    //         }
                    //         drawShaderProgramMenu(shaderMenu, shaderChoices, shaderRegPtr, materialCachePtr, inspectorEngPtr, loggerPtr);
                    //     }
                    
                    //     ImGui::Unindent(theme.indentSize);
                    // }
        
                    ImGui::Unindent(theme.indentSize);
                }
                ImGui::EndChild();
                ImGui::PopStyleColor(3);
                ImGui::PopStyleColor();
                ImGui::PopStyleVar(2);
                ImGui::PopID();
                i++;
                if (i < modelCachePtr->getNumberOfModels()) {
                    ImGui::Dummy(ImVec2(0, 2));
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0, 2));
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

// void ObjectsInspectorUI::drawAddObjectMenu(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr) {

//     const auto& programs = shaderRegPtr->getPrograms();
//     if (programs.empty()) return;

//     ShaderProgram& defaultProgram = *programs.begin()->second;

//     if (ImGui::Button("Add Plane")) {
//         unsigned int id = modelCachePtr->createPreset(ModelType::PlanePreset);
//         // modelCachePtr->getModel(id)->setModelProgram(defaultProgram.name);
//         inspectorEngPtr->refreshUniforms();
//     }
//     if (ImGui::Button("Add Pyramid")) {
//         unsigned int id = modelCachePtr->createPreset(ModelType::PyramidPreset);
//         // modelCachePtr->getModel(id)->setModelProgram(defaultProgram.name);
//         inspectorEngPtr->refreshUniforms();
//     }
//     if (ImGui::Button("Add Cube")) {
//         unsigned int id = modelCachePtr->createPreset(ModelType::CubePreset);
//         // modelCachePtr->getModel(id)->setModelProgram(defaultProgram.name);
//         inspectorEngPtr->refreshUniforms();
//     }
// }

void ObjectsInspectorUI::initializeMenu(MaterialShaderMenu& menu, const std::vector<const char*>& shaderChoices, Logger* loggerPtr, ShaderRegistry* shaderRegPtr, MaterialCache* materialCachePtr) {
    int i = 0;
    if (!materialCachePtr->contains(menu.matID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "ObjectsInspectorUI::initializeMenu (ModelShaderMenu)", "couldn't find material: " + std::to_string(menu.matID));
        return;
    }
    Material* mat  = materialCachePtr->getMaterial(menu.matID);

    for (auto& shaderName : shaderChoices) {
        const ShaderProgram* shader = shaderRegPtr->getProgram(shaderName);
        if (shader == nullptr) {
            continue;
        }

        ShaderProgram* modelProgram = shaderRegPtr->getProgram(mat->getProgramID());
        if (modelProgram == nullptr) {
            menu.selection = 0;
            menu.initialized = true;
            return;
        }

        if (shaderName == modelProgram->name) {
            menu.selection = i;
            menu.initialized = true;
        }
        i++;
    }
    menu.initialized = true;
}

void ObjectsInspectorUI::initializeMenu(ModelTextureMenu& menu, Logger* loggerPtr) {
    loggerPtr->addLog(LogLevel::CRITICAL, "intializeMenu:ModelTextureMenu", "Shouldn not be calling this function! it doesn't work right now");
}

// ALECS ATTEMPT
bool ObjectsInspectorUI::drawMeshesMenu(Model* currModel, MaterialCache* materialCachePtr, ModelCache* modelCachePtr, Logger* loggerPtr) {
    bool isOpen = ImGui::CollapsingHeader("Meshes");
    if (!isOpen) return false;

    ImGui::Indent(theme.indentSize);
    std::vector<unsigned int> materialIDs = materialCachePtr->getAllMaterialIDs();
    // ugly but it keeps the names accessible since matName vec stores pointers
    std::vector<std::string> materialStrings;
    materialStrings.reserve(materialIDs.size());
    std::vector<const char*> materialNames;
    for (unsigned int materialID : materialIDs) {
        materialStrings.push_back(materialCachePtr->getMaterial(materialID)->getName());
        materialNames.push_back(materialStrings.back().c_str());
    }
    
    for (auto& meshInstance : currModel->getMeshInstances()) {
        int selectedItem;
        for (unsigned int i = 0; i < materialIDs.size(); i++) {
            if (meshInstance.materialID == materialIDs[i]) {
                selectedItem = i;
                break;
            }
        }
        std::string label = "##" + std::to_string(meshInstance.meshIdx);
        ImGui::Text("Mesh %u", meshInstance.meshIdx + 1);
        ImGui::SameLine();
        if (ImGui::Combo(label.c_str(), &selectedItem, materialNames.data(), (int)materialNames.size())) {
            unsigned int assignedMaterialID = materialIDs[selectedItem];
            modelCachePtr->changeMeshMaterial(currModel->ID, meshInstance.meshIdx, assignedMaterialID);
        }
    }

    ImGui::Unindent(theme.indentSize);
    return true;
}

bool ObjectsInspectorUI::drawInstancesMenu(Model* model, ModelCache* modelCachePtr, Logger* loggerPtr) {
    bool isOpen = ImGui::CollapsingHeader("Instances");
    if (!isOpen) return false;

    int numOfInstances = model->getInstanceCount();
    ImGui::Text("Number of Instances:");
    ImGui::SameLine();

    ImGui::SetNextItemWidth(110.0f);
    ImGui::InputInt("##InstanceCount", &numOfInstances);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (numOfInstances > 0) {
            model->setInstanceCount(numOfInstances);
        }
    }

    const std::vector<InstanceData>& instanceData = model->getInstanceData();
    for (unsigned int idx = 0; idx < model->getInstanceCount(); idx++) {
        ImGui::PushID(idx);

        InstanceData currInstanceData = instanceData[idx];
        ImGui::Text("%s", ("Instance " + std::to_string(idx + 1)).c_str());
        ImGui::SameLine();
        if (ImGui::DragFloat3("##Position##xx", &currInstanceData.pos.x, .05f)) {
            model->setInstancePosition(idx, currInstanceData.pos);
        }
        

        ImGui::PopID();
    }
    return true;
}

// bool ObjectsInspectorUI::drawShaderProgramMenu(MaterialShaderMenu& menu, const std::vector<const char*>& shaderChoices, ShaderRegistry* shaderRegPtr, MaterialCache* materialCachePtr, InspectorEngine* inspectorEngPtr, Logger* logger) {
//     bool changed = false;
//     ImGui::Text("Material %u", menu.matID);
//     ImGui::SameLine();
//     if (ImGui::Combo(("##" + std::to_string(menu.matID)).c_str(), &menu.selection, shaderChoices.data(), (int)shaderChoices.size())) {
//         changed = true;
//     }

//     if (!changed) return false;

//     ShaderProgram& selectedShader = *shaderRegPtr->getProgram(shaderChoices[menu.selection]);
//     Material* mat = materialCachePtr->getMaterial(menu.matID);
//     if (mat == nullptr) {
//         logger->addLog(LogLevel::LOG_ERROR, "drawShaderProgramMenu", "material not found: " + std::to_string(menu.matID));
//     }
//     mat->setProgramID(selectedShader.name);
//     inspectorEngPtr->refreshUniforms();
//     return true;
// }

bool ObjectsInspectorUI::drawTextureMenu(ModelTextureMenu& menu, Logger* loggerPtr, TextureRegistry* textureRegPtr) {
    // bool changed = false;
    std::vector<const char*> textureChoices;
    const std::vector<const Texture*>& registryTextures = textureRegPtr->readTextures();
    textureChoices.reserve(registryTextures.size());
    for (const Texture* tex : registryTextures) {
        textureChoices.push_back(tex->getPath().c_str());
    }

    if (ImGui::Combo("Texture", &menu.textureSelection, textureChoices.data(), (int)textureChoices.size())) {
        // changed = true;
        loggerPtr->addLog(LogLevel::INFO, "Inspector UI Draw Texture Menu", "changed texture");
    }
    if (ImGui::InputInt("Unit", &menu.unitSelection)) {
        // changed = true;
        loggerPtr->addLog(LogLevel::INFO, "Inspector UI Draw Texture Menu", "changed unit");
    }
    if (drawTextInput(&menu.uniformName, "Uniform Name")) {
        // changed = true;
        loggerPtr->addLog(LogLevel::INFO, "Inspector UI Draw Texture Menu", "changed uniform name");
    }

    return false;
}

bool ObjectsInspectorUI::drawTextInput(std::string* value, const char* label) {
    bool changed = false;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer))) {
        *value = buffer;
        changed = true;
    }
    return changed;
}

bool ObjectsInspectorUI::drawModelPositionInput(Model& model) {
    bool changed = false;
    ImGui::PushID(&model);

    glm::vec3 position = model.getPosition();
    ImGui::Text("Position");
    ImGui::SameLine();
    changed |= ImGui::DragFloat3("##Position##xx", &position.x, .05f);

    ImGui::PopID();
    if (changed) model.setPosition(position);
    return changed;
}

bool ObjectsInspectorUI::drawModelScaleInput(Model& model) {
    bool changed = false;
    ImGui::PushID(&model);

    glm::vec3 scale = model.getScale();
    ImGui::Text("Scale");
    ImGui::SameLine();
    changed |= ImGui::DragFloat3("##Scale##xx", &scale.x,  .05f);

    ImGui::PopID();
    if (changed) model.setScale(scale);
    return changed;
}

bool ObjectsInspectorUI::drawModelOrientationInput(Model& model) {
    bool changed = false;
    ImGui::PushID(&model);

    glm::vec4 rotation = model.getRotation();
    ImGui::Text("Orientation");
    ImGui::SameLine();
    changed |= ImGui::DragFloat4("##Orientation##xx", &rotation.x,  .05f);

    ImGui::PopID();
    if (changed) model.setRotation(rotation.x, glm::vec3(rotation.y, rotation.z, rotation.w));
    return changed;
}

bool ObjectsInspectorUI::drawModelHeader(Model* model, ModelCache* modelCachePtr) {
    unsigned int modelID = model->getID();
    std::string label = model->getName() + "##" + std::to_string(modelID);

    bool isOpen = ImGui::CollapsingHeader(label.c_str()); 

    if (ImGui::BeginPopupContextItem(("Context##" + std::to_string(modelID)).c_str())) {
        if (ImGui::Selectable("Rename")) {
            renamingModelID = modelID; 
            std::snprintf(renameBuffer, sizeof(renameBuffer), "%s", model->getName().c_str()); 
        }
        // TODO: if we want to add these
        if (ImGui::Selectable("Duplicate")) {

        }

        if (ImGui::Selectable("Remove")) {

        }

        ImGui::EndPopup(); 
    }

    if (renamingModelID == modelID) ImGui::OpenPopup("Rename Object"); 

    if (ImGui::BeginPopupModal("Rename Object", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New name for %s:", model->getName().c_str());
        
        bool enterPressed = ImGui::InputText("##NewName", renameBuffer, sizeof(renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
        
        if (ImGui::Button("Save", ImVec2(120, 0)) || enterPressed) {
            modelCachePtr->changeModelName(modelID, std::string(renameBuffer));
            renamingModelID = UINT_MAX; 
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            renamingModelID = UINT_MAX; // Reset state
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    return isOpen;
}