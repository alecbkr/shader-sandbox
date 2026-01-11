#pragma once 
#include "UniformTypes.hpp"
#include <unordered_map>
#include <string>

#define UNIFORM_REGISTRY UniformRegistry::instance()

class UniformRegistry {
    public:
    static UniformRegistry& instance();
    const Uniform* tryReadUniform(const std::string& objectName, const std::string& uniformName) const; // return false if we didn't find it.
    bool containsObject(const std::string& objectName);
    bool containsUniform(const std::string& objectName, const std::string& uniformName);
    const std::unordered_map<std::string, Uniform>* tryReadUniforms(const std::string& objectName) const;
    void registerUniform(const std::string& objectName, Uniform uniform);
    void insertUniformMap(const std::string& objectName, const std::unordered_map<std::string, Uniform>& map);
    void eraseUniform(const std::string& objectName, const std::string& uniformName);

    private:
    UniformRegistry();
    std::unordered_map<std::string, std::unordered_map<std::string, Uniform>> uniforms;
};
