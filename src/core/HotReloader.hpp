#ifndef HOTRELOADER_HPP
#define HOTRELOADER_HPP

#include <string>

class HotReloader{
public:
    static bool initialize();
    static void update();
    static bool compile(const std::string &filepath, const std::string &programName);

private:
    static std::string readSourceFile(const std::string &filepath);
    void scanSourceFiles(const std::string &sourceCode);
    static bool attemptCompile(const std::string &fragShaderPath, const std::string &programName);
};

#endif 
