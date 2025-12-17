#include "UniformRegistry.hpp"
#include <iostream>

UniformRegistry::UniformRegistry() {}

/*
Uniform UniformRegistry::getUniform(std::string shaderProgramName, std::string uniformName) {
    if (uniforms.count(shaderProgramName) > 0 && uniforms.at(shaderProgramName).count(uniformName) > 0) {
        return uniforms.at(shaderProgramName).at(uniformName);
    }
    else {

    }
    auto programPair = uniforms.find(programName);
    if (programPair == uniforms.end()) {
        std::cout << "Set Uniform: No program with that name found in Inspector Engine uniforms" << std::endl;
        return;
    }

    auto& programUniforms = programPair->second;
    
    auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        std::cout << "Set Uniform: No uniform with that name found in Inspector Engine uniforms" << std::endl;
        return;
    }
}
*/

// returns nullptr if uniform doesn't exist
const Uniform* UniformRegistry::tryReadUniform(const std::string& shaderProgramName, const std::string& uniformName) const {
    const auto& programPair = uniforms.find(shaderProgramName);
    if (programPair == uniforms.end()) {
        std::cout << "tryReadUniform: No program with that name found in Uniform Registry" << std::endl;
        return nullptr;
    }

    auto& programUniforms = programPair->second;
    
    const auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        std::cout << "tryReadUniform: No uniform with that name found in Uniform Registry" << std::endl;
        return nullptr;
    }

    return &uniformPair->second;
}

bool UniformRegistry::containsUniform(const std::string& shaderProgramName, const std::string& uniformName) {
    const auto& programPair = uniforms.find(shaderProgramName);
    if (programPair == uniforms.end()) {
        return false;
    }

    auto& programUniforms = programPair->second;
    
    const auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        return false;
    }

    return true;
}

// returns nullptr if uniform doesn't exist
const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadUniforms(const std::string& shaderProgramName) const {
    if (uniforms.count(shaderProgramName) <= 0) {
        std::cout << "tryReadUniform: No program with that name found in Uniform Registry" << std::endl; return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[shaderProgramName];
    const std::unordered_map<std::string, Uniform> *programUniforms = &(uniforms.at(shaderProgramName));

    return programUniforms;
}

void UniformRegistry::registerUniform(const std::string& shaderProgramName, const std::string& uniformName, Uniform uniformValue) {
    uniforms[shaderProgramName][uniformName] = uniformValue;
}

void UniformRegistry::insertUniformMap(const std::string& shaderProgramName, const std::unordered_map<std::string, Uniform>& map) {
    uniforms[shaderProgramName] = map;
}

void UniformRegistry::eraseUniform(const std::string& shaderProgramName, const std::string& uniformName) {
    if (uniforms.count(shaderProgramName) <= 0) {
        std::cout << "eraseUniform: No program with that name found in Uniform Registry" << std::endl;
        return;
    }
    
    if (uniforms.at(shaderProgramName).count(uniformName) <= 0) {
        std::cout << "eraseUniform: No uniform with that name found in Uniform Registry, no need to erase" << std::endl;
        return;
    }
    uniforms.at(shaderProgramName).erase(uniformName);
}

