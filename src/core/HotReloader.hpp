#ifndef HOTRELOADER_HPP
#define HOTRELOADER_HPP

#include <string>
#include "core/logging/Logger.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "object/ModelCache.hpp"
#include "core/EditorEngine.hpp"
#include "core/InspectorEngine.hpp"


class HotReloader{
public:
    HotReloader();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, ModelCache* _modelCachePtr, EditorEngine* _editorEngPtr, InspectorEngine* _inspectorEngPtr);
    void shutdown();
    void update();
    bool compile(const std::string &filepath, const std::string &programName);

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    EditorEngine* editorEngPtr = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;

    std::string readSourceFile(const std::string &filepath);
    void scanSourceFiles(const std::string &sourceCode);
    bool attemptCompile(const std::string &fragShaderPath, const std::string &programName);
};

#endif 
