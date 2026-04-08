#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <filesystem>

#include "application/Project.hpp"
#include "engine/ShaderProgram.hpp"

class Logger;
struct Project;
using ShaderFactoryFn = std::function<ShaderProgram*(const char* vert, const char* frag, const char* name, const unsigned int ID, Logger* logger)>;

class ShaderRegistry {
public:
    ShaderRegistry();
    bool initialize(Logger* _loggerPtr, Project* project, bool registerDefaults = true);
    void shutdown();
    bool registerProgram(const std::filesystem::path& vertex_file, const std::filesystem::path& fragment_file, const std::string& programName);
    ShaderProgram* getProgram(const unsigned int ID) const;
    ShaderProgram* getProgram(const std::string& name) const;
    std::string getProgramName(const unsigned int ID) const;
    void replaceProgram(const unsigned int ID, std::unique_ptr<ShaderProgram> newProgram);
    // void replaceProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);
    const std::unordered_map<unsigned int, std::unique_ptr<ShaderProgram>>& getPrograms() const;
    size_t getNumberOfPrograms() const;
    void setFactory(ShaderFactoryFn fn);
    unsigned int getAndUpdateNextID();
private:
    unsigned int nextID = 0;
    std::unordered_map<std::string, unsigned int> nameToIDMap;
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    Project* project = nullptr;
    ShaderFactoryFn factory_;
};
