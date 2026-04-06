#ifndef HOTRELOADER_HPP
#define HOTRELOADER_HPP

#include <string>

class Logger;
class EventDispatcher;
class ShaderRegistry;
class ModelCache;
class EditorEngine;
class InspectorEngine;
class ContextManager;

class HotReloader{
public:
    HotReloader();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, ShaderRegistry* _shaderRegPtr, ModelCache* _modelCachePtr, EditorEngine* _editorEngPtr, InspectorEngine* _inspectorEngPtr, ContextManager* _contextManagerPtr);
    void shutdown();
    void update();
    bool compile(const std::string &filepath, const std::string &programName, const unsigned int progID);

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    EventDispatcher* eventsPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    EditorEngine* editorEngPtr = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;
    ContextManager* contextManagerPtr = nullptr;


    std::string readSourceFile(const std::string &filepath);
    void scanSourceFiles(const std::string &sourceCode);
    bool attemptCompile(const std::string &fragShaderPath, const std::string &programName, const unsigned int progID);
};

#endif 
