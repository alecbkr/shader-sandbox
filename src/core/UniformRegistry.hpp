#pragma once 
#include "UniformTypes.hpp"
#include <unordered_map>
#include <string>
#include "core/logging/Logger.hpp"

#define UNIFORM_REGISTRY UniformRegistry::instance()

class UniformRegistry {
    public:
    UniformRegistry();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    const Uniform* tryReadUniform(unsigned int modelID, const std::string& uniformName) const; // return false if we didn't find it.
    bool containsObject(unsigned int modelID);
    bool containsUniform(unsigned int modelID, const std::string& uniformName);
    const std::unordered_map<std::string, Uniform>* tryReadUniforms(unsigned int modelID) const;
    void registerUniform(unsigned int modelID, Uniform uniform);
    void insertUniformMap(unsigned int modelID, const std::unordered_map<std::string, Uniform>& map);
    void eraseUniform(unsigned int modelID, const std::string& uniformName);

    private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    std::unordered_map<unsigned int, std::unordered_map<std::string, Uniform>> uniforms;
};
