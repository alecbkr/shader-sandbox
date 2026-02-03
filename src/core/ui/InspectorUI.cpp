#include "core/ui/InspectorUI.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/TextureRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/UniformTypes.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "engine/Errorlog.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/Model.hpp"
#include "object/ModelCache.hpp"
#include "object/Texture.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core/FileRegistry.hpp"
#include "presets/PresetAssets.hpp"

int InspectorUI::height = 400;
int InspectorUI::width = 400;
std::vector<std::string> InspectorUI::uniformNamesToDelete{};
std::string InspectorUI::newUniformName{};
std::string InspectorUI::newUniformShaderName{};
UniformType InspectorUI::newUniformType = UniformType::NoType;
std::unordered_map<unsigned int, ModelShaderMenu> InspectorUI::modelShaderMenus{};
std::unordered_map<unsigned int, ModelTextureMenu> InspectorUI::modelTextureMenus{};
std::unordered_map<std::string, ShaderLinkMenu> InspectorUI::shaderLinkMenus{};

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
    for (auto &[modelID, model] : ModelCache::modelIDMap) {
        if (model->getProgram() == nullptr) {
            // show models with no shader...
            continue;
        }
        std::string label = "model " + std::to_string(modelID);
        if (ImGui::TreeNode(label.c_str())) {
            const std::unordered_map<std::string, Uniform>* uniformMap = UNIFORM_REGISTRY.tryReadUniforms(modelID);

            if (uniformMap == nullptr) {
                Errorlog::getInstance().logEntry(EL_WARNING, "drawUniformInspector", "Object not found in registry: ", modelID);
                ImGui::TreePop();
                continue;
            }

            for (auto &[uniformName, uniformRef] : *uniformMap) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = uniformRef;
                drawUniformInput(uniformCopy, modelID);
                ImGui::PopID();
                imGuiID++;
            }
            ImGui::TreePop();
        }
    }
}

void InspectorUI::drawObjectsInspector() {
    // This is messy. I need to split it up into different functions at some point.
    // something like "setupObjectsInspector"
    drawAddObjectMenu();
    for (auto &[modelID, model] : ModelCache::modelIDMap) {
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
            if (ImGui::TreeNode("mesh")) {
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("textures")) {
                if (!textureMenu.initialized) {
                    //initializeMenu(textureMenu);
                }
                //drawTextureMenu(textureMenu);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("shader program")) {
                std::vector<const char *> shaderChoices;
                shaderChoices.reserve(ShaderRegistry::getNumberOfPrograms());
                auto& shaders = ShaderRegistry::getPrograms();
                for (auto &[name, shader] : shaders) {
                    shaderChoices.push_back(name.c_str());
                }
                if (!shaderMenu.initialized) {
                    initializeMenu(shaderMenu, shaderChoices);
                }
                drawShaderProgramMenu(shaderMenu, shaderChoices);
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
    std::unordered_map<std::string, ShaderProgram*>& programs = ShaderRegistry::getPrograms();
    if (programs.empty()) return;

    // just grab a random shader program it really does not matter
    ShaderProgram& defaultProgram = *programs.begin()->second;

    if (ImGui::Button("Add Plane")) {
        unsigned int planeID = ModelCache::createModel(gridPlane_verts, gridPlane_indices, true, false, true);
        ModelCache::setProgram(planeID, defaultProgram);
        InspectorEngine::refreshUniforms();
    }
    if (ImGui::Button("Add Pyramid")) {
        unsigned int pyramidID = ModelCache::createModel(pyramidVerts, pyramidIndices, true, false, true);
        ModelCache::setProgram(pyramidID, defaultProgram);
        InspectorEngine::refreshUniforms();
    }
    if (ImGui::Button("Add Cube")) {
        unsigned int cubeID = ModelCache::createModel(cubeVerts, cubeIndices, true, false, true);
        ModelCache::setProgram(cubeID, defaultProgram);
        InspectorEngine::refreshUniforms();
    }}

void InspectorUI::drawAssetsInspector() {
    for (const Texture* texPtr : TextureRegistry::readTextures()) {
        ImGui::Text("%s\n", texPtr->path.c_str());
    }
}

void drawRenameFileEntry(ShaderFile* fileData) {
    static char buf[256];
    strncpy(buf, fileData->renameBuffer.c_str(), 255);

    bool keyboardSubmitted = ImGui::InputText(("##FileNameInput" + fileData->fileName).c_str(), buf, 256, ImGuiInputTextFlags_EnterReturnsTrue);
    fileData->renameBuffer = buf;

    ImGui::SameLine();

    bool buttonSumitted = ImGui::Button(("OK##" + fileData->fileName).c_str());

    if ((keyboardSubmitted || buttonSumitted) && !fileData->renameBuffer.empty()) {
        EventDispatcher::TriggerEvent(Event { RenameFile, false, RenameFilePayload {fileData->fileName, fileData->renameBuffer} });
    }

    ImGui::SameLine();

    if (ImGui::Button(("CANCEL##" + fileData->fileName).c_str())) {
        fileData->state = NONE;
        fileData->renameBuffer = fileData->fileName;
    }
}

void drawDeleteFileEntity(ShaderFile* fileData) {
    ImGui::Text("%s", fileData->fileName.c_str());

    ImGui::SameLine();

    if (ImGui::Button(("DELETE##" + fileData->fileName).c_str())) {
        EventDispatcher::TriggerEvent(Event { DeleteFile, false, DeleteFilePayload { fileData->fileName } });
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

void drawStandardFileEntry(ShaderFile* fileData) {
    if (ImGui::Selectable(fileData->fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
            EventDispatcher::TriggerEvent(
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
    std::string path = "../shaders/";

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    ImGui::Text("--------------");
    ImGui::Text("Shader Files");
    ImGui::Text("--------------");
    FileRegistry::reloadMap();

    for (const auto &[fileName, fileData] : FileRegistry::getFiles()) {
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

    
    ImGui::Text("--------------");
    ImGui::Text("Shader Programs");
    ImGui::Text("--------------");
    static int newShaders = 0;
    if (ImGui::Button("+")) {
        shaderLinkMenus.emplace(std::make_pair("myShader_" + std::to_string(newShaders), ShaderLinkMenu{
            .shaderName = "myShader_" + std::to_string(newShaders),
            .initialized = false
        }));
        newShaders++;
    }
    drawShaderLinkMenus(shaderLinkMenus);

    ImGui::PopStyleVar();
}

// Need to set this to multiple functions, it's confusing & inefficient
void InspectorUI::drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus) {
    for (const auto &[shaderName, shader] : ShaderRegistry::getPrograms()) {
        if (!menus.contains(shaderName)) {
            menus[shaderName] = ShaderLinkMenu{
                .shaderName = shaderName,
                .initialized = false,
            };
        }
    }

    // These need to contain empty string first! if you have an issue, check stuff relating to this.
    // TODO: this doesn't work, there's going to be state issues when we add or delete files. We also have no way of maintaining order! 
    std::vector<const char*> vertChoices{""};
    std::vector<const char*> fragChoices{""};
    std::vector<const char*> geoChoices{""}; // not doing this right now

    for (const auto& [id, file] : FileRegistry::getFiles()) {
        std::string& extension = file->extension;
        std::string& filePath = file->filePath;
        if (extension == ".vert") {
            vertChoices.push_back(filePath.c_str());
        }
        else if (extension == ".frag") {
            fragChoices.push_back(filePath.c_str());
        }
        else {
            Logger::addLog(LogLevel::WARNING, "drawShaderLinkMenu", "Shader file type " + extension + " not supported, only .vert and .frag");
        }
    }


    ImGuiID guiID = 0;
    for (auto& [shaderName, menu] : menus) {
        if (!menu.initialized) {
            initializeMenu(menu, vertChoices, geoChoices, fragChoices);
        }
        
        if (ImGui::TreeNode(menu.shaderName.c_str())) {
            ImGui::PushID(guiID);
            drawShaderLinkMenu(menu, vertChoices, geoChoices, fragChoices);
            ImGui::PopID();
            guiID++;
            ImGui::TreePop();
        }
    }
}

void InspectorUI::initializeMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices) {
    const ShaderProgram *oldProgram = ShaderRegistry::getProgram(menu.shaderName);
    bool isNewProgram = oldProgram == nullptr;
    if (isNewProgram) {
        menu.vertSelection = 0;
        menu.fragSelection = 0;
        menu.geometrySelection = 0;
        menu.initialized = true; 
        return;
    }

    for (int i = 0; i < vertChoices.size(); i++) {
        std::string filePath = vertChoices[i];
        if (oldProgram->vertPath == filePath) {
            menu.vertSelection = i;
        }
    }
    for (int i = 0; i < fragChoices.size(); i++) {
        std::string filePath = fragChoices[i];
        if (oldProgram->fragPath == filePath) {
            menu.fragSelection = i;
        }
    }
    menu.geometrySelection = 0;
    menu.initialized = true; 
}

void InspectorUI::initializeMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices) {
    int i = 0;
    if (!ModelCache::modelIDMap.contains(menu.modelID)) {
        Logger::addLog(LogLevel::ERROR, "initializeMenu:ModelShaderMenu", "couldn't find model: " + std::to_string(menu.modelID));
    }
    Model& model = *ModelCache::modelIDMap[menu.modelID];

    for (auto& shaderName: shaderChoices) {
        const ShaderProgram* shader = ShaderRegistry::getProgram(shaderName);
        if (shader == nullptr) {
            // might log this later? idk
            continue;
        }

        if (shader->ID == model.getProgram()->ID) {
            menu.selection = i;
            menu.initialized = true;
        }
        i++;
    }
    menu.initialized = true; 
}

void InspectorUI::initializeMenu(ModelTextureMenu& menu) {
    Logger::addLog(LogLevel::CRITICAL, "intializeMenu:ModelTextureMenu", "Shouldn not be calling this function! it doesn't work right now");
    /*
    Model& model = *ModelCache::modelIDMap[menu.modelID];
    std::unordered_set<GLuint> objectTextureIDs;
    for (const TextureBind& bind : model.?) {
        Logger::addLog(LogLevel::INFO, "initializeMenu: ObjectTextureMenu", ("objTexID: " + std::to_string(bind.texture->ID)).c_str());
        objectTextureIDs.emplace(bind.texture->ID);
    }
    int i = 0;
    for (const Texture* const tex : TextureRegistry::readTextures()) {
        if (objectTextureIDs.contains(tex->ID)) {
            menu.textureSelection = i; 
            break;
        }
        i++;
    }
    menu.initialized = true;
    */
}

void InspectorUI::drawShaderLinkMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices) {
    // for  conciseness, put newSelector logic up here into a separate variable.
    bool changed = false;

    if (ImGui::Combo("Vertex Shader", &menu.vertSelection, vertChoices.data(), (int)vertChoices.size())) { 
        changed = true; 
    }
    if (ImGui::Combo("Geometry Shader", &menu.geometrySelection, geoChoices.data(), (int)geoChoices.size())) {
        changed = true; 
    }
    if (ImGui::Combo("Fragment Shader", &menu.fragSelection, fragChoices.data(), (int)fragChoices.size())) {
        changed = true; 
    }

    bool validSelection = menu.fragSelection != 0 && menu.vertSelection != 0 && menu.shaderName != "";
    if (validSelection) {
        ImGui::Text("Valid");
    }
    else {
        ImGui::Text("Invalid! Using old program..."); 
    }
    if (validSelection && changed) {
        const std::string vert = vertChoices[menu.vertSelection];
        const std::string frag = fragChoices[menu.fragSelection];
        const std::string& name = menu.shaderName;
        InspectorEngine::handleEditShaderProgram(vert, frag, name);
    }
}

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

bool InspectorUI::drawShaderProgramMenu(ModelShaderMenu& menu, const std::vector<const char*>& shaderChoices) {
    bool changed = false;

    // display combo box
    if (ImGui::Combo("Shader", &menu.selection, shaderChoices.data(),
                        (int)shaderChoices.size())) {
        changed = true;
    }

    if (!changed) return false;
    
    // add check in case we get more types
    ShaderProgram& selectedShader = *ShaderRegistry::getProgram(shaderChoices[menu.selection]);
    ModelCache::setProgram(menu.modelID, selectedShader); 
    InspectorEngine::refreshUniforms();
    return true;
}

bool InspectorUI::drawTextureMenu(ModelTextureMenu& menu) {
    // Not sure what the deal is here yet...
    bool changed = false;
    std::vector<const char *> textureChoices;
    const std::vector<const Texture*>& registryTextures = TextureRegistry::readTextures();
    textureChoices.reserve(registryTextures.size());
    for (const Texture* tex : registryTextures) {
        textureChoices.push_back(tex->path.c_str());
    }


    if (ImGui::Combo("Texture", &menu.textureSelection, textureChoices.data(),
                        (int)textureChoices.size())) {
        changed = true;
        std::cout << "changed texture" << std::endl;
    }
    if (ImGui::InputInt("Unit", &menu.unitSelection)) {
        changed = true;
        std::cout << "changed unit" << std::endl;
    }
    if (drawTextInput(&menu.uniformName, "Uniform Name")) {
        changed = true;
        std::cout << "changed uniform name" << std::endl;
    }
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

bool InspectorUI::drawUniformInputValue(int* value, Uniform* uniform) {
    return ImGui::InputInt("value", value);
}

bool InspectorUI::drawUniformInputValue(float* value, Uniform* uniform) {
    return ImGui::InputFloat("value", value);
}


bool InspectorUI::drawUniformInputValue(glm::vec3* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        ImGui::Checkbox("Use Color Picker", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        changed |= ImGui::ColorPicker3("", &value->x);
    }
    else {
        changed |= ImGui::InputFloat("x", &value->x);
        changed |= ImGui::InputFloat("y", &value->y);
        changed |= ImGui::InputFloat("z", &value->z);
    }
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::vec4* value, Uniform* uniform) {
    bool changed = false;
    bool useColorPicker = false;
    if (uniform != nullptr) {
        ImGui::Checkbox("Use Color Picker", &uniform->useAlternateEditor);
        useColorPicker = uniform->useAlternateEditor;
    }

    if (useColorPicker) {
        changed |= ImGui::ColorPicker4("", &value->x);
    }
    else {
        changed |= ImGui::InputFloat("x", &value->x);
        changed |= ImGui::InputFloat("y", &value->y);
        changed |= ImGui::InputFloat("z", &value->z);
        changed |= ImGui::InputFloat("w", &value->w);
    }
    return changed; 
}

bool InspectorUI::drawUniformInputValue(glm::quat* value, Uniform* uniform) {
    bool changed = false;
    changed |= ImGui::InputFloat("x", &value->x);
    changed |= ImGui::InputFloat("y", &value->y);
    changed |= ImGui::InputFloat("z", &value->z);
    changed |= ImGui::InputFloat("w", &value->w);
    return changed;
}

bool InspectorUI::drawUniformInputValue(glm::mat4* value, Uniform* uniform) {
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

bool InspectorUI:: drawUniformInputValue(InspectorSampler2D* value, Uniform* uniform) {
    return drawUniformInputValue(&value->textureUnit);
}

void InspectorUI::drawUniformInput(Uniform& uniform, unsigned int modelID) {
    if (ImGui::TreeNode(uniform.name.c_str())) {
        bool changed = false;
        std::visit([&](auto& val){
            changed = drawUniformInputValue(&val, &uniform);
        }, uniform.value);

        if (changed) {
            InspectorEngine::applyInput(modelID, uniform);
        }

        /*
        if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
            uniformNamesToDelete.push_back(uniform.name);
        }
        */
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
