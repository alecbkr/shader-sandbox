#ifndef HOTRELOADER_HPP
#define HOTRELOADER_HPP

#include <string>
#include "core/ShaderHandler.hpp"

class InspectorEngine;

class HotReloader{

public:
    static bool compile(const std::string &filepath, const std::string &programName);

private:
    static std::string readSourceFile(const std::string &filepath);
    static void scanSourceFiles(const std::string &sourceCode);
    static bool attemptCompile(const std::string &fragShaderPath, const std::string &programName);
};

#endif 
