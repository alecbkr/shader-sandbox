#pragma once
#include "object/MaterialCache.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <memory>
#include <filesystem>
class Logger;
class InspectorEngine;
class TextureRegistry;
class TextureCache;
class ShaderRegistry;
class UniformRegistry;
class FileRegistry;
class EventDispatcher;
class ModelCache;
class UniformInspectorUI;
class ObjectsInspectorUI;
class MaterialsInspectorUI;
class AssetsInspectorUI;
class FileInspectorUI;
class Fonts;
struct Project;
struct SettingsStyles;

class InspectorUI {
public:
    InspectorUI();
    ~InspectorUI();
    bool initialize(Logger* _loggerPtr, InspectorEngine* _inspectorEngPtr, TextureRegistry* _textureRegPtr, TextureCache* _textureCachePtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, FileRegistry* _fileRegPtr, MaterialCache* _materialCachePtr, Fonts* _fontsPtr, Project* _project, SettingsStyles* _styles);
    void shutdown();
    void render();
  
private:
    bool intitialized = false;
    Logger* loggerPtr = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;
    TextureRegistry* textureRegPtr = nullptr;
    TextureCache* textureCachePtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    FileRegistry* fileRegPtr = nullptr;
    MaterialCache* materialCachePtr = nullptr;
    Fonts* fontsPtr = nullptr;
    SettingsStyles* stylesPtr = nullptr;
    std::unique_ptr<UniformInspectorUI> uniformInspectorUI;
    std::unique_ptr<ObjectsInspectorUI> objectsInspectorUI;
    std::unique_ptr<AssetsInspectorUI> assetsInspectorUI;
    std::unique_ptr<FileInspectorUI> fileInspectorUI;
    std::unique_ptr<MaterialsInspectorUI> materialsInspectorUI;
    int height = 0;
    int width = 0;
};
