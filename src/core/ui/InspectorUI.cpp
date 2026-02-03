#include "core/ui/InspectorUI.hpp"

#include <filesystem>

#include "core/logging/Logger.hpp"
#include "core/InspectorEngine.hpp"
#include "core/TextureRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/UniformTypes.hpp"
#include "core/UniformRegistry.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/Errorlog.hpp"
#include "object/ModelCache.hpp"
#include "object/Texture.hpp"
#include "core/FileRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>


InspectorUI::InspectorUI() {
    uniformNamesToDelete.clear();
    newUniformName = "";
    newUniformShaderName = "";
    newUniformType = UniformType::NoType;
    modelShaderSelectors.clear();
    modelTextureSelectors.clear();
    intitialized = false;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    textureRegPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    fileRegPtr = nullptr;
}

bool InspectorUI::initialize(Logger* _loggerPtr, InspectorEngine* _inspectorEngPtr, TextureRegistry* _textureRegPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, FileRegistry* _fileRegPtr) {
    if (intitialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Inspector UI Initialization", "Inspector UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    inspectorEngPtr = _inspectorEngPtr;
    textureRegPtr = _textureRegPtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    eventsPtr = _eventsPtr;
    modelCachePtr = _modelCachePtr;
    fileRegPtr = _fileRegPtr;
    intitialized = true;
    return true;
}

void InspectorUI::shutdown() {
    if (!intitialized) return;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    textureRegPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    intitialized = false;
}

void InspectorUI::render() {
    float menuBarHeight = ImGui::GetFrameHeight();
    
    int displayWidth = ImGui::GetIO().DisplaySize.x;
    int displayHeight = ImGui::GetIO().DisplaySize.y - menuBarHeight;
    int targetWidth = (float)displayWidth * 0.2f;
    int targetHeight = (float)displayHeight * 1.0f;
    
    int offsetX = displayWidth * 0.8f;
    
    ImGui::SetNextWindowSize(ImVec2(targetWidth, targetHeight + 1), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(offsetX, menuBarHeight), ImGuiCond_Always);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Inspector", nullptr, flags)) {

        if (ImGui::BeginTabBar("Inspector tabs")) {
            
            if (ImGui::BeginTabItem("Uniforms")) {
                drawUniformInspector();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Objects")) {
                drawObjectsInspector();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Assets")) {
                drawAssetsInspector();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shader Files")) {
                drawShaderFileInspector();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

void InspectorUI::drawUniformInspector() {
    int imGuiID = 0;
    for (auto &[modelID, model] : modelCachePtr->modelIDMap) {
        std::string label = "model " + std::to_string(modelID);
        if (ImGui::TreeNode(label.c_str())) {
            const std::unordered_map<std::string, Uniform>* uniformMap = uniformRegPtr->tryReadUniforms(modelID);

            if (uniformMap == nullptr) {
                Errorlog::getInstance().logEntry(EL_WARNING, "drawUniformInspector", "Object not found in registry: ", modelID);
                continue;
            }

            for (auto &[uniformName, uniformRef] : *uniformMap) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = uniformRef;
                drawUniformInput(uniformCopy, modelID);
                ImGui::PopID();
                imGuiID++;
            }
            for (std::string uniformName : uniformNamesToDelete)
                uniformRegPtr->eraseUniform(modelID, uniformName);
            uniformNamesToDelete.clear();

            ImGui::TreePop();
        }
    }
}

void InspectorUI::drawObjectsInspector() {
    // TODO: an arbitrary high number, just somewhere above the uniform inspector's ids.
    int imGuiID = 100000;
    drawAddObjectMenu();
    for (auto &[modelID, model] : modelCachePtr->modelIDMap) {
        if (!modelShaderSelectors.contains(modelID)) {
            modelShaderSelectors[modelID] = ObjectShaderSelector{ 
                .modelID = modelID,
                .selection = 0,
            };    
        }
        if (!modelTextureSelectors.contains(modelID)) {
            modelTextureSelectors[modelID] = ObjectTextureSelector{ 
                .modelID = modelID,
                .uniformName = "baseTex",
                .textureSelection = 0,
            };    
        }
        ObjectShaderSelector& shaderSelector = modelShaderSelectors[modelID];
        ObjectTextureSelector& textureSelector = modelTextureSelectors[modelID];

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
            if (ImGui::TreeNode("mesh")) {

                ImGui::TreePop();
            }
            if (ImGui::TreeNode("textures")) {
                drawTextureSelector(textureSelector);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("shader program")) {
                drawShaderProgramSelector(shaderSelector);
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
}

void InspectorUI::drawAddObjectMenu() {
    static const std::vector<float> gridPlane_verts {
        -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 1.0f
    };

    static const std::vector<unsigned int> gridPlane_indices {
        0, 1, 2, 
        0, 2, 3
    };

    // PYRAMID
    static const std::vector<float> pyramidVerts = {
        // Base
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, // 0: bottom-left
        0.5f, 0.0f, -0.5f,  1.0f, 0.0f, // 1: bottom-right
        0.5f, 0.0f,  0.5f,  1.0f, 1.0f, // 2: top-right
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, // 3: top-left

        // Apex
        0.0f, 1.0f, 0.0f,   0.5f, 0.5f  // 4: tip
    };

     
    static const std::vector<unsigned int> pyramidIndices = {
        0, 1, 2,  0, 2, 3, // base
        0, 1, 4,            // side 1
        1, 2, 4,            // side 2
        2, 3, 4,            // side 3
        3, 0, 4             // side 4
    };

    // CUBE
    static const std::vector<float> cubeVerts = {
        // positions       // UVs
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
        0.5f,-0.5f,-0.5f, 1.0f,0.0f,
        0.5f, 0.5f,-0.5f, 1.0f,1.0f,
        -0.5f, 0.5f,-0.5f, 0.0f,1.0f,

        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
        0.5f,-0.5f, 0.5f, 1.0f,0.0f,
        0.5f, 0.5f, 0.5f, 1.0f,1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f,1.0f
    };

    // Indices for cube (two triangles per face)
    static const std::vector<unsigned int> cubeIndices = {
        0,1,2, 0,2,3, // back
        4,5,6, 4,6,7, // front
        3,2,6, 3,6,7, // top
        0,1,5, 0,5,4, // bottom
        1,2,6, 1,6,5, // right
        0,3,7, 0,7,4  // left
    };
    std::unordered_map<std::string, ShaderProgram*>& programs = shaderRegPtr->getPrograms();
    if (programs.empty()) return;

    // just grab a random shader program it really does not matter
    ShaderProgram& defaultProgram = *programs.begin()->second;

    if (ImGui::Button("Add Plane")) {
        unsigned int planeID = modelCachePtr->createModel(gridPlane_verts, gridPlane_indices, true, false, true);
        modelCachePtr->setProgram(planeID, defaultProgram);
        inspectorEngPtr->refreshUniforms();
    }
    if (ImGui::Button("Add Pyramid")) {
        unsigned int pyramidID = modelCachePtr->createModel(pyramidVerts, pyramidIndices, true, false, true);
        modelCachePtr->setProgram(pyramidID, defaultProgram);
        inspectorEngPtr->refreshUniforms();
    }
    if (ImGui::Button("Add Cube")) {
        unsigned int cubeID = modelCachePtr->createModel(cubeVerts, cubeIndices, true, false, true);
        modelCachePtr->setProgram(cubeID, defaultProgram);
        inspectorEngPtr->refreshUniforms();
    }
}

void InspectorUI::drawAssetsInspector() {
    for (const Texture* texPtr : textureRegPtr->readTextures()) {
        ImGui::Text("%s\n", texPtr->path.c_str());
    }
}

void InspectorUI::drawRenameFileEntry(ShaderFile* fileData) {
    static char buf[256];
    strncpy(buf, fileData->renameBuffer.c_str(), 255);

    bool keyboardSubmitted = ImGui::InputText(("##FileNameInput" + fileData->fileName).c_str(), buf, 256, ImGuiInputTextFlags_EnterReturnsTrue);
    fileData->renameBuffer = buf;

    ImGui::SameLine();

    bool buttonSumitted = ImGui::Button(("OK##" + fileData->fileName).c_str());

    if ((keyboardSubmitted || buttonSumitted) && !fileData->renameBuffer.empty()) {
        eventsPtr->TriggerEvent(Event { RenameFile, false, RenameFilePayload {fileData->fileName, fileData->renameBuffer} });
    }

    ImGui::SameLine();

    if (ImGui::Button(("CANCEL##" + fileData->fileName).c_str())) {
        fileData->state = NONE;
        fileData->renameBuffer = fileData->fileName;
    }
}

void InspectorUI::drawDeleteFileEntity(ShaderFile* fileData) {
    ImGui::Text(fileData->fileName.c_str());

    ImGui::SameLine();

    if (ImGui::Button(("DELETE##" + fileData->fileName).c_str())) {
        eventsPtr->TriggerEvent(Event { DeleteFile, false, DeleteFilePayload { fileData->fileName } });
    }

    ImGui::SameLine();

    if (ImGui::Button(("CANCEL##" + fileData->fileName).c_str())) {
        fileData->state = NONE;
    }
}

void drawContextMenu(ShaderFile* fileData) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Rename")) {
            fileData->state = RENAME;
        }
        if (ImGui::Selectable("Delete")) {
            fileData->state = DELETE;
        }

        ImGui::EndPopup();
    }
}

void InspectorUI::drawStandardFileEntry(ShaderFile* fileData) {
    if (ImGui::Selectable(fileData->fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
            eventsPtr->TriggerEvent(
                Event{
                    OpenFile,
                    false,
                    OpenFilePayload{ fileData->filePath, fileData->fileName }
                }
            );
        }
    }
    drawContextMenu(fileData);
}

void InspectorUI::drawShaderFileInspector() {
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    fileRegPtr->reloadMap();

    for (const auto &[fileName, fileData] : fileRegPtr->getFiles()) {
        switch (fileData->state) {
            case RENAME:
                drawRenameFileEntry(fileData);
                break;
            case DELETE:
                drawDeleteFileEntity(fileData);
                break;
            case NEW:
                break;
            case NONE: default:
                drawStandardFileEntry(fileData);
                break;
        }
    }

    ImGui::PopStyleVar();
}

/*
// this is from an old version
// keeping the code cause it's a nice template. get rid of it once we finish the inspector MVP or if we have the code elsewhere.
void InspectorUI::drawAddUniformMenu() {
    ImGui::Text("Add Uniform");
    drawTextInput(&newUniformName, "Uniform Name");

    // Build a list of shader names
    std::vector<const char *> shaderChoices;
    shaderChoices.reserve(shaderRegPtr->getNumberOfPrograms() + 1);
    shaderChoices.push_back("");

    auto& shaders = shaderRegPtr->getPrograms();
    for (auto &[name, shader] : shaders) {
        shaderChoices.push_back(name.c_str());
    }
    // Keep track of the selected shader index
    static int shaderChoice = 0;
    if (newUniformShaderName == "")
        shaderChoice = 0;
    // Display combo box
    if (ImGui::Combo("Shader", &shaderChoice, shaderChoices.data(),
                        (int)shaderChoices.size())) {
        // When selection changes, update newUniformShaderName
        newUniformShaderName = shaderChoices[shaderChoice];
    }
    // Dropdown menu from enum
    const char *typeNames[6] = {"None", "Int",  "Float",
                                "Vec3", "Vec4", "Reference"};
    int choice = static_cast<int>(newUniformType);
    if (ImGui::Combo("Uniform Type", &choice, typeNames,
                        IM_ARRAYSIZE(typeNames))) {
        newUniformType = static_cast<UniformType>(choice);
    }

    if (ImGui::Button("Add Uniform", ImVec2(100, 25))) {
        bool uniqueValidUniform = false;
        bool validShaderName = shaders.count(newUniformShaderName) >= 1;
        if (validShaderName) {
            uniqueValidUniform =
                uniformRegPtr->containsUniform(newUniformShaderName, newUniformName) &&
                newUniformType != UniformType::NoType;
        }

        if (uniqueValidUniform) {
            Uniform newUniform;
            newUniform.name = newUniformName;
            newUniform.type = newUniformType;

            switch (newUniformType) {
            case UniformType::Int:
                newUniform.value = 0;
                break;
            case UniformType::Float:
                newUniform.value = 0.0f;
                break;
            case UniformType::Vec3:
                newUniform.value = glm::vec3(0.0f);
                break;
            case UniformType::Vec4:
                newUniform.value = glm::vec4(0.0f);
                break;
            case UniformType::Mat4:
                newUniform.value = glm::mat4(0.0f);
                break;
            default:
                std::cout << "invalid new uniform type, making it an int"
                            << std::endl;
                newUniform.type = UniformType::Int;
                newUniform.value = 0;
                break;
            }

            uniformRegPtr->registerUniform(newUniformShaderName, newUniform);
            newUniformShaderName = "";
            newUniformName = "";
        } else {
            std::cout << "bad new uniform input" << std::endl;
        }
    }
}
*/

bool InspectorUI::drawTextInput(std::string *value, const char *label) {
    bool changed = false;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());
    if (ImGui::InputText(label, buffer, sizeof(buffer))) {
        *value = buffer;
        changed = true;
    }
    return changed;
}

bool InspectorUI::drawShaderProgramSelector(ObjectShaderSelector& selector) {
    bool changed = false;
    std::vector<const char *> shaderChoices;
    shaderChoices.reserve(shaderRegPtr->getNumberOfPrograms());

    auto& shaders = shaderRegPtr->getPrograms();
    for (auto &[name, shader] : shaders) {
        shaderChoices.push_back(name.c_str());
    }
    // display combo box
    if (ImGui::Combo("Shader", &selector.selection, shaderChoices.data(),
                        (int)shaderChoices.size())) {
        changed = true;
    }

    if (!changed) return false;
    
    // add check in case we get more types
    ShaderProgram& selectedShader = *shaderRegPtr->getProgram(shaderChoices[selector.selection]);
    modelCachePtr->setProgram(selector.modelID, selectedShader); 
    inspectorEngPtr->refreshUniforms();
    return true;
}

bool InspectorUI::drawTextureSelector(ObjectTextureSelector& selector) {
    // bool changed = false;
    // std::vector<const char *> textureChoices;
    // std::vector<const Texture*> textures = TextureRegistry::readTextures();
    // textureChoices.reserve(textures.size());
    // for (const Texture* tex : textures) {
    //     textureChoices.push_back(tex->path.c_str());
    // }
    // if (ImGui::Combo("Texture", &selector.textureSelection, textureChoices.data(),
    //                     (int)textureChoices.size())) {
    //     changed = true;
    //     std::cout << "changed texture" << std::endl;
    // }
    // if (ImGui::InputInt("Unit", &selector.unitSelection)) {
    //     changed = true;
    //     std::cout << "changed unit" << std::endl;
    // }
    // if (drawTextInput(&selector.uniformName, "Uniform Name")) {
    //     changed = true;
    //     std::cout << "changed uniform name" << std::endl;
    // }

    // if (!changed) return false;
    
    // // add check in case we get more types
    // const Texture* selectedTexture = textures.at(selector.textureSelection);
    // ModelCache::setTexture(selector.modelID, *selectedTexture, selector.unitSelection, selector.uniformName); 

    // return true;

    return false; //TEMP ADDING AND CHANGING TEXTURES DOESNT CURRENTLY WORK
}

bool InspectorUI::drawUniformInputValue(int* value) {
    return ImGui::InputInt("value", value);
}

bool InspectorUI::drawUniformInputValue(float* value) {
    return ImGui::InputFloat("value", value);
}

bool InspectorUI::drawUniformInputValue(glm::vec3* value) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::vec4* value) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    changed |= ImGui::InputFloat("w", &value->w);
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::quat* value) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    changed |= ImGui::InputFloat("w", &value->w);
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::mat4* value) {
    static int tableID = 0;
    const int columns = 4;
    tableID++;
    bool changed = false;
    if (ImGui::BeginTable(("Matrix4x4" + std::to_string(tableID)).c_str(), columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        for (int row = 0; row < 4; ++row) {
            ImGui::TableNextRow();
            for (int col = 0; col < 4; ++col) {
                ImGui::TableSetColumnIndex(col);
                ImGui::PushID(row * 4 + col); // unique ID
                ImGui::PushItemWidth(60);
                ImGui::InputFloat("", &(*value)[col][row]); // column-major
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }    
    return changed;
}

void InspectorUI::drawUniformInput(Uniform& uniform, unsigned int modelID) {
    if (ImGui::TreeNode(uniform.name.c_str())) {
        bool changed = false;
        std::visit([&](auto& val){
            changed = drawUniformInputValue(&val);
        }, uniform.value);

        if (changed) {
            inspectorEngPtr->applyInput(modelID, uniform);
        }

        if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
            uniformNamesToDelete.push_back(uniform.name);
        }
        ImGui::TreePop();
    }
}


bool InspectorUI::drawModelPositionInput(Model* model) {
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


bool InspectorUI::drawModelScaleInput(Model* model) {
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


bool InspectorUI::drawModelOrientationInput(Model* model) {
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