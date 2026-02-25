#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include "engine/ShaderProgram.hpp"

class Logger;
using ShaderFactoryFn = std::function<ShaderProgram*(const char* vert, const char* frag, const char* name, Logger* logger)>;

class ShaderRegistry {
public:
    ShaderRegistry();
    bool initialize(Logger* _loggerPtr, bool registerDefaults = true);
    void shutdown();
    bool registerProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);
    ShaderProgram* getProgram(const std::string& programName) const;
    //std::unordered_map<std::string, ShaderProgram>& getPrograms();
    void replaceProgram(const std::string &programName, std::unique_ptr<ShaderProgram> newProgram);
    const std::unordered_map<std::string, std::unique_ptr<ShaderProgram>>& getPrograms() const;
    size_t getNumberOfPrograms() const;
    void setFactory(ShaderFactoryFn fn);
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> programs;
    ShaderFactoryFn factory_;
};
