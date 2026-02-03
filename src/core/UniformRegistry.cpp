#include "UniformRegistry.hpp"
#include "engine/Errorlog.hpp"
#include <iostream>
#include "core/logging/Logger.hpp"


UniformRegistry::UniformRegistry() {
    initialized = false;
    uniforms.clear();
}

bool UniformRegistry::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Uniform Registry Initialization", "Uniform Registry was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    uniforms.clear();
    initialized = true;
    return true;
}

void UniformRegistry::shutdown() {
    loggerPtr = nullptr;
    uniforms.clear();
    initialized = false;
}

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
const Uniform* UniformRegistry::tryReadUniform(unsigned int modelID, const std::string& uniformName) const {
    const auto& programPair = uniforms.find(modelID);
    if (programPair == uniforms.end()) {
        Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniform:", "no object found with ID", static_cast<int>(modelID));
        return nullptr;
    }

    auto& programUniforms = programPair->second;
    
    const auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniform", "No object found in Uniform Registry with ID ", modelID);
        return nullptr;
    }

    return &uniformPair->second;
}
bool UniformRegistry::containsObject(unsigned int modelID) {
    return uniforms.contains(modelID);
}

bool UniformRegistry::containsUniform(unsigned int modelID, const std::string& uniformName) {
    const auto& programPair = uniforms.find(modelID);
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
const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadUniforms(unsigned int modelID) const {
    if (uniforms.count(modelID) <= 0) {
        Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID", modelID );
        return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[modelID];
    const std::unordered_map<std::string, Uniform> *programUniforms = &(uniforms.at(modelID));

    return programUniforms;
}

void UniformRegistry::registerUniform(unsigned int modelID, Uniform uniform) {
    uniforms[modelID][uniform.name] = uniform;
}

void UniformRegistry::insertUniformMap(unsigned int modelID, const std::unordered_map<std::string, Uniform>& map) {
    uniforms[modelID] = map;
}

void UniformRegistry::eraseUniform(unsigned int modelID, const std::string& uniformName) {
    if (uniforms.count(modelID) <= 0) {
        Errorlog::getInstance().logEntry(EL_WARNING, "eraseUniform", "No object found in Uniform Registry with ID ", modelID);
        return;
    }
    
    if (uniforms.at(modelID).count(uniformName) <= 0) {
        Errorlog::getInstance().logEntry(EL_WARNING, "eraseUniform", ("No uniform with name " + uniformName + " found in Uniform Registry").c_str());
        return;
    }
    uniforms.at(modelID).erase(uniformName);
}



