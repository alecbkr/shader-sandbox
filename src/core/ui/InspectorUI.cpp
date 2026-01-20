#include "core/ui/InspectorUI.hpp"

#include <filesystem>

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
#include "object/ObjCache.hpp"
#include "object/Texture.hpp"
#include <memory>
#include <ostream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

int InspectorUI::height = 400;
int InspectorUI::width = 400;
std::vector<std::string> InspectorUI::uniformNamesToDelete{};
std::string InspectorUI::newUniformName{};
std::string InspectorUI::newUniformShaderName{};
UniformType InspectorUI::newUniformType = UniformType::NoType;
std::unordered_map<std::string, ObjectShaderSelector> InspectorUI::objectShaderSelectors{};
std::unordered_map<std::string, ObjectTextureSelector> InspectorUI::objectTextureSelectors{};
std::unordered_map<std::string, ShaderLinkMenu> InspectorUI::shaderPrograms{};
ShaderLinkMenu InspectorUI::linkNewShaderMenu = ShaderLinkMenu{
    .shaderName = "",
    .vertSelection = 0,
    .geometrySelection = 0,
    .fragSelection = 0,
};

void InspectorUI::render() {
    ImGui::Text("Inspector");
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

void InspectorUI::drawUniformInspector() {
    int imGuiID = 0;
    for (auto &[objectName, object] : ObjCache::objMap) {
        if (ImGui::TreeNode(objectName.c_str())) {
            const std::unordered_map<std::string, Uniform>* uniformMap = UNIFORM_REGISTRY.tryReadUniforms(objectName);

            if (uniformMap == nullptr) {
                Errorlog::getInstance().logEntry(EL_WARNING, "drawUniformInspector", ("Object not found in registry: " + objectName).c_str());
                continue;
            }

            for (auto &[uniformName, uniformRef] : *uniformMap) {
                ImGui::PushID(imGuiID);
                Uniform uniformCopy = uniformRef;
                drawUniformInput(uniformCopy, objectName);
                ImGui::PopID();
                imGuiID++;
            }
            for (std::string uniformName : uniformNamesToDelete)
                UNIFORM_REGISTRY.eraseUniform(objectName, uniformName);
            uniformNamesToDelete.clear();

            ImGui::TreePop();
        }
    }
}

void InspectorUI::drawObjectsInspector() {
    // TODO: an arbitrary high number, just somewhere above the uniform inspector's ids.
    int imGuiID = 100000;
    drawAddObjectMenu();
    for (auto &[objectName, object] : ObjCache::objMap) {
        // When we initialize new objects, their selection values start at 0.
        // We need to set them manually in their draw___Selector functions
        if (!objectShaderSelectors.contains(objectName)) {
            objectShaderSelectors[objectName] = ObjectShaderSelector{ 
                .objectName = objectName,
                .selection = 0,
                .newSelector = true,
            };    
        }
        if (!objectTextureSelectors.contains(objectName)) {
            objectTextureSelectors[objectName] = ObjectTextureSelector{ 
                .objectName = objectName,
                .uniformName = "baseTex",
                .textureSelection = 0,
                .unitSelection = 0,
                .newSelector = true,
            };    
        }
        ObjectShaderSelector& shaderSelector = objectShaderSelectors[objectName];
        ObjectTextureSelector& textureSelector = objectTextureSelectors[objectName];

        if (ImGui::TreeNode(objectName.c_str())) {
            if (ImGui::TreeNode("position")) {
                drawUniformInputValue(&object->objPosition);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("scale")) {
                drawUniformInputValue(&object->objScale);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("orientation")) {
                drawUniformInputValue(&object->objOrientation);
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

    static const std::vector<int> gridPlane_indices {
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

     
    static const std::vector<int> pyramidIndices = {
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
    static const std::vector<int> cubeIndices = {
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

    int objectCount = ObjCache::getNumberOfObjects();

    if (ImGui::Button("Add Plane")) {
        ObjCache::createObj(("Plane_" + std::to_string(objectCount)).c_str(), gridPlane_verts, gridPlane_indices, false, true, defaultProgram);
        InspectorEngine::refreshUniforms();
    }
    if (ImGui::Button("Add Pyramid")) {
        ObjCache::createObj(("Pyramid_" + std::to_string(objectCount)).c_str(), pyramidVerts, pyramidIndices, false, true, defaultProgram);
        InspectorEngine::refreshUniforms();
    }
    if (ImGui::Button("Add Cube")) {
        ObjCache::createObj(("Cube_" + std::to_string(objectCount)).c_str(), cubeVerts, cubeIndices, false, true, defaultProgram);
        InspectorEngine::refreshUniforms();
    }
}

void InspectorUI::drawAssetsInspector() {
    for (const Texture* texPtr : TextureRegistry::readTextures()) {
        ImGui::Text("%s\n", texPtr->path.c_str());
    }
}


void InspectorUI::drawShaderFileInspector() {

    std::string path = "../shaders/";

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    ImGui::Text("--------------");
    ImGui::Text("Shader Programs");
    ImGui::Text("--------------");
    drawShaderLinkMenu(linkNewShaderMenu, ShaderLinkMenuType::Create);
    for (const auto & [shaderName, shaderProgram] : ShaderRegistry::getPrograms()) {
        if (!shaderPrograms.contains(shaderName)) {
            shaderPrograms[shaderName] = ShaderLinkMenu{ 
                .shaderName = shaderName,
                .vertSelection = 0,
                .geometrySelection = 0,
                .fragSelection = 0,
                .newSelector = true
            };    
        }
        if (ImGui::TreeNode(shaderName.c_str())) {
            drawShaderLinkMenu(shaderPrograms[shaderName], ShaderLinkMenuType::Edit);
            ImGui::TreePop();
        }
    }
    ImGui::Text("");

    ImGui::Text("--------------");
    ImGui::Text("Shader Files");
    ImGui::Text("--------------");
    for (const auto & dirEntry : std::filesystem::directory_iterator(path)) {
        std::string filePath = dirEntry.path().string();
        std::string fileName = dirEntry.path().filename().string();
        if (ImGui::Selectable(fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                EventDispatcher::TriggerEvent(
                    Event{
                        OpenFile,
                        false,
                        OpenFilePayload{ filePath, fileName }
                    }
                );
            }
        }
    }

    ImGui::PopStyleVar();
}

void InspectorUI::drawShaderLinkMenu(ShaderLinkMenu& menu, ShaderLinkMenuType type) {
    // for  conciseness, put newSelector logic up here into a separate variable.
    const bool isEditor = type == ShaderLinkMenuType::Edit;
    const bool isNewEditor = isEditor && menu.newSelector;
    menu.newSelector = false;
    const ShaderProgram *oldProgram;
    if (isEditor) {
        oldProgram = ShaderRegistry::getProgram(menu.shaderName);
        if (oldProgram == nullptr) {
            Logger::addLog(LogLevel::ERROR, "drawShaderLinkMenu", "Couldn't find shader program with name " + menu.shaderName);
            return;
        }
    }
    bool changed = false;

    const std::string path = "../shaders/";
    std::vector<std::string> files;
    std::vector<std::string> extensions;
    for (const auto & dirEntry : std::filesystem::directory_iterator(path)) {
        files.push_back(dirEntry.path());
        extensions.push_back(dirEntry.path().extension());
    }

    std::vector<const char*> vertChoices{""};
    std::vector<const char*> fragChoices{""};
    std::vector<const char*> geoChoices{""}; // not doing this right now

    if (files.size() != extensions.size()) {
        Logger::addLog(LogLevel::ERROR, "drawShaderLinkMenu", "files & extensions vectors have different sizes. this should never happen");
        return;
    }

    int vertIndex = 1;
    int fragIndex = 1;
    for (int i = 0; i < files.size() && i < extensions.size(); i++) {
        const std::string& extension = extensions[i];
        const std::string& filePath = files[i];
        if (extension == ".vert") {
            vertChoices.push_back(filePath.c_str());
            if (isNewEditor && oldProgram->vertPath == filePath) {
                 menu.vertSelection = vertIndex;
            }
            vertIndex++;
        }
        else if (extension == ".frag") {
            fragChoices.push_back(filePath.c_str());
            if (isNewEditor && oldProgram->fragPath == filePath) {
                 menu.fragSelection = fragIndex;
            }
            fragIndex++;
        }
        else {
            Logger::addLog(LogLevel::WARNING, "drawShaderLinkMenu", "Shader file type " + extension + " not supported, only .vert and .frag");
        }
    }

    if (ImGui::Combo("Vertex Shader", &menu.vertSelection, vertChoices.data(), (int)vertChoices.size())) { 
        changed = true; 
    }
    if (ImGui::Combo("Geometry Shader", &menu.geometrySelection, geoChoices.data(), (int)geoChoices.size())) {
        changed = true; 
    }
    if (ImGui::Combo("Fragment Shader", &menu.fragSelection, fragChoices.data(), (int)fragChoices.size())) {
        changed = true; 
    }

    char buffer[256];
    if (type == ShaderLinkMenuType::Create && ImGui::InputText("New Program Name", buffer, sizeof(buffer))) {
        menu.shaderName = buffer;
    }


    bool validSelection = menu.fragSelection != 0 && menu.vertSelection != 0 && menu.shaderName != "";
    if (validSelection && ImGui::Button("Link Shader Program")) {
        const std::string vert = vertChoices[menu.vertSelection];
        const std::string frag = fragChoices[menu.fragSelection];
        const std::string& name = menu.shaderName;
        if (type == ShaderLinkMenuType::Create) {
            ShaderRegistry::registerProgram(vert, frag, name);
        }
        else if (type == ShaderLinkMenuType::Edit) {
            ShaderRegistry::replaceProgram(name, vert, frag);
        }
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

bool InspectorUI::drawShaderProgramSelector(ObjectShaderSelector& selector) {
    Object& object = *ObjCache::objMap[selector.objectName];
    bool changed = false;

    std::vector<const char *> shaderChoices;
    shaderChoices.reserve(ShaderRegistry::getNumberOfPrograms());
    auto& shaders = ShaderRegistry::getPrograms();
    int i = 0;
    for (auto &[name, shader] : shaders) {
        shaderChoices.push_back(name.c_str());

        if (selector.newSelector && shader->ID == object.getProgramID()) {
            selector.selection = i;
            selector.newSelector = false;
        }
        i++;
    }
    // display combo box
    if (ImGui::Combo("Shader", &selector.selection, shaderChoices.data(),
                        (int)shaderChoices.size())) {
        changed = true;
    }

    if (!changed) return false;
    
    ShaderProgram& selectedShader = *ShaderRegistry::getProgram(shaderChoices[selector.selection]);
    ObjCache::setProgram(selector.objectName, selectedShader); 
    InspectorEngine::refreshUniforms();
    return true;
}

bool InspectorUI::drawTextureSelector(ObjectTextureSelector& selector) {
    bool changed = false;
    std::vector<const char *> textureChoices;
    const std::vector<const Texture*>& registryTextures = TextureRegistry::readTextures();
    textureChoices.reserve(registryTextures.size());
    for (const Texture* tex : registryTextures) {
        textureChoices.push_back(tex->path.c_str());
    }

    // need to set the initial state of the selector if it's new
    if (selector.newSelector) {
        Logger::addLog(LogLevel::INFO, "drawTextureSelector", "new selector");
        std::cout << "hi" << std::endl;
        Object& object = *ObjCache::objMap[selector.objectName];
        std::unordered_set<GLuint> objectTextureIDs;
        for (const TextureBind& bind : object.renderable.mat.textures) {
            Logger::addLog(LogLevel::INFO, "drawTextureSelector", ("objTexID: " + std::to_string(bind.texture->ID)).c_str());
            objectTextureIDs.emplace(bind.texture->ID);
        }
        int i = 0;
        for (const Texture* const tex : registryTextures) {
            Logger::addLog(LogLevel::INFO, "drawTextureSelector", ("texID" + std::to_string(i) + ": " + std::to_string(tex->ID)).c_str());
            if (objectTextureIDs.contains(tex->ID)) {
                Logger::addLog(LogLevel::INFO, "drawTextureSelector", ("Found texID: " + std::to_string(tex->ID)).c_str());
                selector.textureSelection = i; 
                break;
            }
            i++;
        }
        selector.newSelector = false;
    }

    if (ImGui::Combo("Texture", &selector.textureSelection, textureChoices.data(),
                        (int)textureChoices.size())) {
        changed = true;
        std::cout << "changed texture" << std::endl;
    }
    if (ImGui::InputInt("Unit", &selector.unitSelection)) {
        changed = true;
        std::cout << "changed unit" << std::endl;
    }
    if (drawTextInput(&selector.uniformName, "Uniform Name")) {
        changed = true;
        std::cout << "changed uniform name" << std::endl;
    }

    if (!changed) return false;
    
    // add check in case we get more types
    const Texture* selectedTexture = registryTextures.at(selector.textureSelection);
    ObjCache::setTexture(selector.objectName, *selectedTexture, selector.unitSelection, selector.uniformName); 

    return true;
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

void InspectorUI::drawUniformInput(Uniform& uniform, const std::string& objectName) {
    if (ImGui::TreeNode(uniform.name.c_str())) {
        bool changed = false;
        std::visit([&](auto& val){
            changed = drawUniformInputValue(&val);
        }, uniform.value);

        if (changed) {
            InspectorEngine::applyInput(objectName, uniform);
        }

        if (ImGui::Button("Delete Uniform", ImVec2(100, 20))) {
            uniformNamesToDelete.push_back(uniform.name);
        }
        ImGui::TreePop();
    }
}
