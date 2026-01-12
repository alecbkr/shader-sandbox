#ifndef HOTRELOADER_HPP
#define HOTRELOADER_HPP

#include <string>
#include "core/ShaderRegistry.hpp"

class InspectorEngine;

class HotReloader{
  
private:
    ShaderRegistry *m_handler;
    InspectorEngine *m_inspector;
    std::string readSourceFile(const std::string &filepath);
    void scanSourceFiles(const std::string &sourceCode);
    bool attemptCompile(const std::string &fragShaderPath, const std::string &programName);

public:
    HotReloader(ShaderRegistry *handler, InspectorEngine *inspector);
    bool compile(const std::string &filepath, const std::string &programName);
};

#endif 
