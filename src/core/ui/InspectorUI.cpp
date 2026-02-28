#include "core/ui/InspectorUI.hpp"

#include "core/ui/UniformInspectorUI.hpp"
#include "core/ui/ObjectsInspectorUI.hpp"
#include "core/ui/AssetsInspectorUI.hpp"
#include "core/ui/FileInspectorUI.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/TextureRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/UniformRegistry.hpp"
#include "core/ShaderRegistry.hpp"
#include "object/ModelCache.hpp"
#include "object/Texture.hpp"
#include "core/FileRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include <string>

InspectorUI::InspectorUI() {
    intitialized = false;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    textureRegPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    fileRegPtr = nullptr;
    height = 0;
    width = 0;
    uniformInspectorUI = std::make_unique<UniformInspectorUI>();
    objectsInspectorUI = std::make_unique<ObjectsInspectorUI>();
    assetsInspectorUI = std::make_unique<AssetsInspectorUI>();
    fileInspectorUI = std::make_unique<FileInspectorUI>();
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

InspectorUI::~InspectorUI() = default;

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
                uniformInspectorUI->draw(loggerPtr, inspectorEngPtr, shaderRegPtr, uniformRegPtr, modelCachePtr);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Objects")) {
                objectsInspectorUI->draw(loggerPtr, inspectorEngPtr, shaderRegPtr, textureRegPtr, modelCachePtr);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Assets")) {
                assetsInspectorUI->draw(textureRegPtr);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shader Files")) {
                fileInspectorUI->draw(loggerPtr, inspectorEngPtr, shaderRegPtr, fileRegPtr, eventsPtr);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

