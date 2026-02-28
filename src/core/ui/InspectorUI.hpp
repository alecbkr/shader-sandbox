#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "object/Model.hpp"
#include "core/UniformTypes.hpp"
#include "core/FileRegistry.hpp"
#include <memory>
class Logger;
class InspectorEngine;
class TextureRegistry;
class ShaderRegistry;
class UniformRegistry;
class EventDispatcher;
class ModelCache;
class UniformInspectorUI;
class ObjectsInspectorUI;
class AssetsInspectorUI;
class FileInspectorUI;

class InspectorUI {
public:
    InspectorUI();
    ~InspectorUI();
    bool initialize(Logger* _loggerPtr, InspectorEngine* _inspectorEngPtr, TextureRegistry* _textureRegPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, FileRegistry* _fileRegPtr);
    void shutdown();
    void render();
  
private:
    bool intitialized = false;
    Logger* loggerPtr = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;
    TextureRegistry* textureRegPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    FileRegistry* fileRegPtr = nullptr;
    std::unique_ptr<UniformInspectorUI> uniformInspectorUI;
    std::unique_ptr<ObjectsInspectorUI> objectsInspectorUI;
    std::unique_ptr<AssetsInspectorUI> assetsInspectorUI;
    std::unique_ptr<FileInspectorUI> fileInspectorUI;
    int height = 0;
    int width = 0;
};
