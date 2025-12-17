#pragma once 
#include "UniformTypes.hpp"
#include <unordered_map>
#include <string>

class UniformRegistry {
    public:
    UniformRegistry();
    const Uniform* tryReadUniform(const std::string& shaderProgramName, const std::string& uniformName) const; // return false if we didn't find it.
    bool containsUniform(const std::string& shaderProgramName, const std::string& uniformName);
    const std::unordered_map<std::string, Uniform>* tryReadUniforms(const std::string& shaderProgramName) const;
    void registerUniform(const std::string& shaderProgramName, const std::string& uniformName, Uniform uniformValue);
    void insertUniformMap(const std::string& shaderProgramName, const std::unordered_map<std::string, Uniform>& map);
    void eraseUniform(const std::string& shaderProgramName, const std::string& uniformName);

    private:
    std::unordered_map<std::string, std::unordered_map<std::string, Uniform>> uniforms;
};
