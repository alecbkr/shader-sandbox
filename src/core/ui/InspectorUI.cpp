#include "core/ui/InspectorUI.hpp"
#include "core/ui/UniformInspectorUI.hpp"
#include "core/ui/ObjectsInspectorUI.hpp"
#include "core/ui/MaterialsInspectorUI.hpp"
#include "core/ui/AssetsInspectorUI.hpp"
#include "core/ui/FileInspectorUI.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/TextureRegistry.hpp"
#include "texture/TextureCache.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EventDispatcher.hpp"
#include "core/UniformRegistry.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/FileRegistry.hpp"
#include "object/MaterialCache.hpp"
#include "object/ModelCache.hpp"
#include "texture/Texture.hpp"
#include "core/FileRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include "application/Project.hpp"
#include "application/SettingsStyles.hpp"
#include "core/ui/Fonts.hpp"
#include <string>

InspectorUI::InspectorUI() {
    intitialized = false;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    textureRegPtr = nullptr;
    textureCachePtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    fileRegPtr = nullptr;
    materialCachePtr = nullptr;
    height = 0;
    width = 0;
}

bool InspectorUI::initialize(Logger* _loggerPtr, InspectorEngine* _inspectorEngPtr, TextureRegistry* _textureRegPtr, TextureCache* _textureCachePtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, FileRegistry* _fileRegPtr, MaterialCache* _materialCachePtr, Fonts* _fontsPtr, Project* _project, SettingsStyles* _styles) {
    if (intitialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Inspector UI Initialization", "Inspector UI was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    inspectorEngPtr = _inspectorEngPtr;
    textureRegPtr = _textureRegPtr;
    textureCachePtr = _textureCachePtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    eventsPtr = _eventsPtr;
    modelCachePtr = _modelCachePtr;
    fileRegPtr = _fileRegPtr;
    assetsInspectorUI = std::make_unique<AssetsInspectorUI>(_fontsPtr, _project, _styles);
    uniformInspectorUI = std::make_unique<UniformInspectorUI>(_styles);
    objectsInspectorUI = std::make_unique<ObjectsInspectorUI>(_styles);
    materialsInspectorUI = std::make_unique<MaterialsInspectorUI>(_fontsPtr, _styles, _materialCachePtr, _textureCachePtr, _shaderRegPtr);
    fileInspectorUI = std::make_unique<FileInspectorUI>();
    materialCachePtr = _materialCachePtr;
    fontsPtr = _fontsPtr;
    stylesPtr = _styles;
    intitialized = true;
    return true;
}

InspectorUI::~InspectorUI() = default;

void InspectorUI::shutdown() {
    if (!intitialized) return;
    loggerPtr = nullptr;
    inspectorEngPtr = nullptr;
    textureRegPtr = nullptr;
    textureCachePtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    eventsPtr = nullptr;
    modelCachePtr = nullptr;
    materialCachePtr = nullptr;
    fontsPtr = nullptr;
    stylesPtr = nullptr;
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
                uniformInspectorUI->draw(loggerPtr, inspectorEngPtr, shaderRegPtr, uniformRegPtr, modelCachePtr, materialCachePtr);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Objects")) {
                objectsInspectorUI->draw(loggerPtr, inspectorEngPtr, shaderRegPtr, textureRegPtr, modelCachePtr, materialCachePtr);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Materials")) {
                materialsInspectorUI->draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Assets")) {
                assetsInspectorUI->draw();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shaders")) {
                fileInspectorUI->draw(loggerPtr, inspectorEngPtr, shaderRegPtr, fileRegPtr, eventsPtr, fontsPtr, stylesPtr);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
