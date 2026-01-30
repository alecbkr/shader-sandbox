#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include "engine/ShaderProgram.hpp"
#include "core/logging/Logger.hpp"

class ShaderRegistry {
public:
    ShaderRegistry();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    bool registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);
    ShaderProgram* getProgram(const std::string& programName);
    //std::unordered_map<std::string, ShaderProgram>& getPrograms();
    void replaceProgram(const std::string &programName, ShaderProgram *newProgram);
    std::unordered_map<std::string, ShaderProgram*> &getPrograms();
    size_t getNumberOfPrograms();
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    std::unordered_map<std::string, ShaderProgram*> programs;
};
