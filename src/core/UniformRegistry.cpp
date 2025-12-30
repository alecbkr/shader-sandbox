#include "UniformRegistry.hpp"
#include "engine/Errorlog.hpp"
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
const Uniform* UniformRegistry::tryReadUniform(const std::string& objectName, const std::string& uniformName) const {
    const auto& programPair = uniforms.find(objectName);
    if (programPair == uniforms.end()) {
        Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniform", ("No object with name " + objectName + " found in Uniform Registry").c_str());
        return nullptr;
    }

    auto& programUniforms = programPair->second;
    
    const auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniform", ("No object with name " + objectName + " found in Uniform Registry").c_str());
        return nullptr;
    }

    return &uniformPair->second;
}

bool UniformRegistry::containsUniform(const std::string& objectName, const std::string& uniformName) {
    const auto& programPair = uniforms.find(objectName);
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
const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadUniforms(const std::string& objectName) const {
    if (uniforms.count(objectName) <= 0) {
        Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", ("No object with name " + objectName + " found in Uniform Registry").c_str());
        return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[objectName];
    const std::unordered_map<std::string, Uniform> *programUniforms = &(uniforms.at(objectName));

    return programUniforms;
}

void UniformRegistry::registerUniform(const std::string& objectName, const std::string& uniformName, Uniform uniformValue) {
    uniforms[objectName][uniformName] = uniformValue;
}

void UniformRegistry::insertUniformMap(const std::string& objectName, const std::unordered_map<std::string, Uniform>& map) {
    uniforms[objectName] = map;
}

void UniformRegistry::eraseUniform(const std::string& objectName, const std::string& uniformName) {
    if (uniforms.count(objectName) <= 0) {
        Errorlog::getInstance().logEntry(EL_WARNING, "eraseUniform", ("No object with name " + objectName + " found in Uniform Registry").c_str());
        return;
    }
    
    if (uniforms.at(objectName).count(uniformName) <= 0) {
        Errorlog::getInstance().logEntry(EL_WARNING, "eraseUniform", ("No uniform with name " + uniformName + " found in Uniform Registry").c_str());
        return;
    }
    uniforms.at(objectName).erase(uniformName);
}

