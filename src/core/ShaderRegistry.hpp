#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include "engine/ShaderProgram.hpp"

class ShaderRegistry {
public:
    static bool initialize();
    static bool registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);
    static ShaderProgram* getProgram(const std::string& programName);
    //static std::unordered_map<std::string, ShaderProgram>& getPrograms();
    static void replaceProgram(const std::string &programName, ShaderProgram *newProgram);
    static std::unordered_map<std::string, ShaderProgram*> &getPrograms();
    static size_t getNumberOfPrograms();
private:
    static std::unordered_map<std::string, ShaderProgram*> programs;
    static bool initialized;
};
