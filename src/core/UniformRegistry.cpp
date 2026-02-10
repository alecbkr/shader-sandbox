#include "UniformRegistry.hpp"
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
        std::cout << "Set Uniform: No uniform with that name found in Inspector Engine uniforms" << std::endl;
        return;
    }
}
*/

// returns nullptr if uniform doesn't exist
const Uniform* UniformRegistry::tryReadUniform(unsigned int modelID, const std::string& uniformName) const {
    const auto& programPair = uniforms.find(modelID);
    if (programPair == uniforms.end()) {
        // this function should be documented properly so that it is known when returning nullptr it means a uniform wasnt found or doesnt exist
        // a more proper fix for this is to return an enum with the proper statuses. (ie. Uniform::NOT_FOUND) or something like this
        return nullptr;
    }

    auto& programUniforms = programPair->second;
    
    const auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        // see previous comment
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
        // this function should be documented properly so that it is known when returning nullptr it means uniforms werent found or dont exist
        // a more proper fix for this is to return an enum with the proper statuses. (ie. Uniform::NOT_FOUND) or something like this
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
    if (uniforms.count(modelID) <= 0) return;
    if (uniforms.at(modelID).count(uniformName) <= 0) return;
    uniforms.at(modelID).erase(uniformName);
}



