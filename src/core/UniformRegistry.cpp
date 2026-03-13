#include "UniformRegistry.hpp"
#include "core/logging/Logger.hpp"


UniformRegistry::UniformRegistry() {
    initialized = false;
    old_version_uniforms.clear();
}

bool UniformRegistry::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Uniform Registry Initialization", "Uniform Registry was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    old_version_uniforms.clear();
    initialized = true;
    return true;
}

void UniformRegistry::shutdown() {
    loggerPtr = nullptr;
    old_version_uniforms.clear();
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
const Uniform* UniformRegistry::tryReadUniform(unsigned int materialID, const std::string& uniformName) const {
    const auto& programPair = old_version_uniforms.find(materialID);
    if (programPair == old_version_uniforms.end()) {
        // this function should be documented properly so that it is known when returning nullptr it means a uniform wasnt found or doesnt exist
        // a more proper fix for this is to return an enum with the proper statuses. (ie. Uniform::NOT_FOUND) or something like this
        return nullptr;
    }

    auto& programUniforms = programPair->second;
    
    if (!programUniforms.contains(uniformName)) {
        // see previous comment
        return nullptr;
    }
    auto id = programUniforms.at(uniformName);
    if (!uniforms.contains(id)) {
        return nullptr;
    }

    return &uniforms.at(id);
}

bool UniformRegistry::containsMaterial(unsigned int matID) {
    return old_version_uniforms.contains(matID);
}

bool UniformRegistry::containsUniform(unsigned int matID, const std::string& uniformName) {
    const auto& programPair = old_version_uniforms.find(matID);
    if (programPair == old_version_uniforms.end()) {
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
const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadUniforms(unsigned int matID) const {
    if (old_version_uniforms.count(matID) <= 0) {
        // this function should be documented properly so that it is known when returning nullptr it means uniforms werent found or dont exist
        // a more proper fix for this is to return an enum with the proper statuses. (ie. Uniform::NOT_FOUND) or something like this
        return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[modelID];
    const std::unordered_map<std::string, Uniform> *programUniforms = &(old_version_uniforms.at(matID));

    return programUniforms;
}

void UniformRegistry::registerInspectorUniform(unsigned int matID, Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    uniform.materialID = matID;
    old_version_uniforms[matID][uniform.name] = uniform.ID;
    uniforms[uniform.ID] = uniform;
}

void UniformRegistry::insertUniformMap(unsigned int matID, const std::unordered_map<std::string, Uniform>& map) {
    old_version_uniforms[matID];
    for (auto& [name, uniformRef] : map) {
        uniforms[nextID] = uniformRef;
        auto& uniform = uniforms[nextID];
        uniform.ID = nextID;
        nextID++;
        uniform.materialID = matID;
        old_version_uniforms[matID][uniform.name] = uniform.ID;
    }
}

void UniformRegistry::eraseUniform(unsigned int matID, const std::string& uniformName) {
    if (!old_version_uniforms.contains(matID)) return;
    if (!old_version_uniforms.at(matID).contains(uniformName)) return;
    unsigned int id = old_version_uniforms.at(matID).at(uniformName);
    if (!uniforms.contains(id)) return;
    uniforms.erase(id);
    old_version_uniforms.at(matID).erase(uniformName);

}

// ALECS TEST JUNK
void UniformRegistry::registerSceneUniform(Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    scene_uniforms[uniform.name] = uniform.ID;
    uniforms[uniform.ID] = uniform;
}


void UniformRegistry::registerModelUniform(unsigned int modelID, Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    model_uniforms[modelID][uniform.name] = uniform.ID;
    uniforms[uniform.ID] = uniform;
}



void UniformRegistry::registerMaterialUniform(unsigned int modelID, unsigned int materialID, Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    material_uniforms[std::make_pair(modelID, materialID)][uniform.name] = uniform.ID;
    uniforms[uniform.ID] = uniform;
}


const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadSceneUniforms() const {
    if (scene_uniforms.size() <= 0) {
        // Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID");
        return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[modelID];
    const std::unordered_map<std::string, Uniform> *sceneUniforms = &(scene_uniforms);

    return sceneUniforms;
}


const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadModelUniforms(unsigned int modelID) const {
    if (model_uniforms.count(modelID) <= 0) {
        // Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID", modelID );
        return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[modelID];
    const std::unordered_map<std::string, Uniform> *modelUniforms = &(model_uniforms.at(modelID));

    return modelUniforms;
}


const std::unordered_map<std::string, Uniform>* UniformRegistry::tryReadMaterialUniforms(unsigned int modelID, unsigned int materialID) const {
    if (material_uniforms.count(std::make_pair(modelID, materialID)) <= 0) {
        // Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID", modelID );
        return nullptr;
    }

    //std::unordered_map<std::string, Uniform>* programUniforms = &uniforms[modelID];
    const std::unordered_map<std::string, Uniform> *materialUniforms = &(material_uniforms.at(std::make_pair(modelID, materialID)));

    return materialUniforms;
}
