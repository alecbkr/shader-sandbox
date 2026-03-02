#include "core/ui/ObjectsInspectorUI.hpp"

#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/TextureRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/Model.hpp"
#include "object/ModelCache.hpp"
#include "presets/PresetAssets.hpp"
#include "texture/Texture.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

void ObjectsInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, TextureRegistry* textureRegPtr, ModelCache* modelCachePtr) {
    drawAddObjectMenu(loggerPtr, inspectorEngPtr, shaderRegPtr, modelCachePtr);
    for (auto& [modelID, model] : modelCachePtr->modelIDMap) {
        if (!modelShaderMenus.contains(modelID)) {
            modelShaderMenus[modelID] = ModelShaderMenu{
                .modelID = modelID,
                .selection = 0,
                .initialized = false,
            };
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
        ModelShaderMenu& shaderMenu = modelShaderMenus[modelID];
        ModelTextureMenu& textureMenu = modelTextureMenus[modelID];

        std::string label = "model " + std::to_string(modelID);
        if (ImGui::TreeNode(label.c_str())) {
            if (ImGui::TreeNode("position")) {
                drawModelPositionInput(model.get());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("scale")) {
                drawModelScaleInput(model.get());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("orientation")) {
                drawModelOrientationInput(model.get());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("textures")) {
                if (!textureMenu.initialized) {
                    // initializeMenu(textureMenu);
                }
                // drawTextureMenu(textureMenu);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("shader program")) {
                std::vector<const char*> shaderChoices;
                shaderChoices.reserve(shaderRegPtr->getNumberOfPrograms());
                const auto& shaders = shaderRegPtr->getPrograms();
                for (auto& [name, shader] : shaders) {
                    shaderChoices.push_back(name.c_str());
                }
                if (!shaderMenu.initialized) {
                    initializeMenu(shaderMenu, shaderChoices, loggerPtr, shaderRegPtr, modelCachePtr);
                }
                drawShaderProgramMenu(shaderMenu, shaderChoices, shaderRegPtr, modelCachePtr, inspectorEngPtr);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
}

void ObjectsInspectorUI::drawAddObjectMenu(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr) {
    static const std::vector<float> gridPlane_verts{
        -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 1.0f
    };

    static const std::vector<unsigned int> gridPlane_indices{
        0, 1, 2,
        0, 2, 3
    };

    static const std::vector<float> pyramidVerts = {
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f,
        0.5f, 0.0f, -0.5f,  1.0f, 0.0f,
        0.5f, 0.0f,  0.5f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,   0.5f, 0.5f
    };

    static const std::vector<unsigned int> pyramidIndices = {
        0, 1, 2,  0, 2, 3,
        0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4
    };

    static const std::vector<float> cubeVerts = {
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
        0.5f,-0.5f,-0.5f, 1.0f,0.0f,
        0.5f, 0.5f,-0.5f, 1.0f,1.0f,
        -0.5f, 0.5f,-0.5f, 0.0f,1.0f,
        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
        0.5f,-0.5f, 0.5f, 1.0f,0.0f,
        0.5f, 0.5f, 0.5f, 1.0f,1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f,1.0f
    };

    static const std::vector<unsigned int> cubeIndices = {
        0,1,2, 0,2,3,
        4,5,6, 4,6,7,
        3,2,6, 3,6,7,
        0,1,5, 0,5,4,
        1,2,6, 1,6,5,
        0,3,7, 0,7,4
    };

    const auto& programs = shaderRegPtr->getPrograms();
    if (programs.empty()) return;

    ShaderProgram& defaultProgram = *programs.begin()->second;

    if (ImGui::Button("Add Plane")) {
        unsigned int id = modelCachePtr->createPreset(MeshPreset::PLANE);
        modelCachePtr->getModel(id)->setModelProgram(defaultProgram.name);
        inspectorEngPtr->refreshUniforms();
    }
    if (ImGui::Button("Add Pyramid")) {
        unsigned int id = modelCachePtr->createPreset(MeshPreset::PYRAMID);
        modelCachePtr->getModel(id)->setModelProgram(defaultProgram.name);
        inspectorEngPtr->refreshUniforms();
    }
    if (ImGui::Button("Add Cube")) {
        unsigned int id = modelCachePtr->createPreset(MeshPreset::CUBE);
        modelCachePtr->getModel(id)->setModelProgram(defaultProgram.name);
        inspectorEngPtr->refreshUniforms();
    }
}

void ObjectsInspectorUI::initializeMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices, Logger* loggerPtr, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr) {
    int i = 0;
    if (!modelCachePtr->modelIDMap.contains(menu.modelID)) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "initializeMenu:ModelShaderMenu", "couldn't find model: " + std::to_string(menu.modelID));
    }
    Model& model = *modelCachePtr->modelIDMap[menu.modelID];

    for (auto& shaderName : shaderChoices) {
        const ShaderProgram* shader = shaderRegPtr->getProgram(shaderName);
        if (shader == nullptr) {
            continue;
        }

        ShaderProgram* modelProgram = shaderRegPtr->getProgram(model.getProgramID());
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

bool ObjectsInspectorUI::drawShaderProgramMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices, ShaderRegistry* shaderRegPtr, ModelCache* modelCachePtr, InspectorEngine* inspectorEngPtr) {
    bool changed = false;
    if (ImGui::Combo("Shader", &menu.selection, shaderChoices.data(), (int)shaderChoices.size())) {
        changed = true;
    }

    if (!changed) return false;

    ShaderProgram& selectedShader = *shaderRegPtr->getProgram(shaderChoices[menu.selection]);
    modelCachePtr->getModel(menu.modelID)->setModelProgram(selectedShader.name);
    inspectorEngPtr->refreshUniforms();
    return true;
}

bool ObjectsInspectorUI::drawTextureMenu(ModelTextureMenu& menu, Logger* loggerPtr, TextureRegistry* textureRegPtr) {
    bool changed = false;
    std::vector<const char*> textureChoices;
    const std::vector<const Texture*>& registryTextures = textureRegPtr->readTextures();
    textureChoices.reserve(registryTextures.size());
    for (const Texture* tex : registryTextures) {
        textureChoices.push_back(tex->path.c_str());
    }

    if (ImGui::Combo("Texture", &menu.textureSelection, textureChoices.data(), (int)textureChoices.size())) {
        changed = true;
        loggerPtr->addLog(LogLevel::INFO, "Inspector UI Draw Texture Menu", "changed texture");
    }
    if (ImGui::InputInt("Unit", &menu.unitSelection)) {
        changed = true;
        loggerPtr->addLog(LogLevel::INFO, "Inspector UI Draw Texture Menu", "changed unit");
    }
    if (drawTextInput(&menu.uniformName, "Uniform Name")) {
        changed = true;
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

bool ObjectsInspectorUI::drawModelPositionInput(Model* model) {
    bool changed = false;
    ImGui::PushID(model);

    glm::vec3 position = model->getPosition();
    changed |= ImGui::InputFloat("x", &position.x);
    changed |= ImGui::InputFloat("y", &position.y);
    changed |= ImGui::InputFloat("z", &position.z);

    ImGui::PopID();
    if (changed) model->setPosition(position);
    return changed;
}

bool ObjectsInspectorUI::drawModelScaleInput(Model* model) {
    bool changed = false;
    ImGui::PushID(model);

    glm::vec3 scale = model->getScale();
    changed |= ImGui::InputFloat("x", &scale.x);
    changed |= ImGui::InputFloat("y", &scale.y);
    changed |= ImGui::InputFloat("z", &scale.z);

    ImGui::PopID();
    if (changed) model->setScale(scale);
    return changed;
}

bool ObjectsInspectorUI::drawModelOrientationInput(Model* model) {
    bool changed = false;
    ImGui::PushID(model);

    glm::vec4 rotation = model->getRotation();
    changed |= ImGui::InputFloat("angle", &rotation.x);
    changed |= ImGui::InputFloat("x-axis", &rotation.y);
    changed |= ImGui::InputFloat("y-axis", &rotation.z);
    changed |= ImGui::InputFloat("z-axis", &rotation.w);

    ImGui::PopID();
    if (changed) model->setRotation(rotation.x, glm::vec3(rotation.y, rotation.z, rotation.w));
    return changed;
}
