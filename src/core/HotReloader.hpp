#ifndef HOTRELOADER_HPP
#define HOTRELOADER_HPP

#include <string>
#include "core/ShaderHandler.hpp"

class InspectorEngine;

class HotReloader{
  
private:
    ShaderHandler *m_handler;
    InspectorEngine *m_inspector;
    std::string readSourceFile(const std::string &filepath);
    void scanSourceFiles(const std::string &sourceCode);
    bool attemptCompile(const std::string &fragShaderPath, const std::string &programName);

public:
    HotReloader(ShaderHandler *handler, InspectorEngine *inspector);
    bool compile(const std::string &filepath, const std::string &programName);
};

#endif 
