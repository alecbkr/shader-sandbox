#include "UniformRegistry.hpp"
#include "core/logging/Logger.hpp"
#include "application/Project.hpp"


UniformRegistry::UniformRegistry() {
    initialized = false;
    materialUniforms.clear();
    sceneUniforms.clear();
    modelUniforms.clear();
}

bool UniformRegistry::initialize(Logger* _loggerPtr, Project* _project) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Uniform Registry Initialization", "Uniform Registry was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    project = _project;
    materialUniforms.clear();
    sceneUniforms.clear();
    modelUniforms.clear();
    initialized = true;
    return true;
}

void UniformRegistry::shutdown() {
    loggerPtr = nullptr;
    materialUniforms.clear();
    sceneUniforms.clear();
    modelUniforms.clear();
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

const std::unordered_map<unsigned int, Uniform>& UniformRegistry::readUniforms() {
    return project->uniforms;
}
const Uniform* UniformRegistry::tryReadUniform(unsigned int id) {
    if (!project->uniforms.contains(id)) {
        return nullptr;
    }
    else return &project->uniforms[id];
}

// returns nullptr if uniform doesn't exist
const Uniform* UniformRegistry::tryReadMaterialUniform(unsigned int materialID, const std::string& uniformName) const {
    const auto& programPair = materialUniforms.find(materialID);
    if (programPair == materialUniforms.end()) {
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
    if (!project->uniforms.contains(id)) {
        return nullptr;
    }

    return &project->uniforms.at(id);
}

bool UniformRegistry::containsMaterial(unsigned int matID) {
    return materialUniforms.contains(matID);
}

bool UniformRegistry::containsMaterialUniform(unsigned int matID, const std::string& uniformName) {
    const auto& programPair = materialUniforms.find(matID);
    if (programPair == materialUniforms.end()) {
        return false;
    }

    auto& programUniforms = programPair->second;
    
    const auto uniformPair = programUniforms.find(uniformName);
    if (uniformPair == programUniforms.end()) {
        return false;
    }

    return true;
}

void UniformRegistry::registerMaterialUniformMap(unsigned int matID, const std::unordered_map<std::string, Uniform>& map) {
    materialUniforms[matID];
    for (auto& [name, uniformRef] : map) {
        project->uniforms[nextID] = uniformRef;
        auto& uniform = project->uniforms[nextID];
        uniform.ID = nextID;
        nextID++;
        uniform.materialID = matID;
        materialUniforms[matID][uniform.name] = uniform.ID;
    }
}

void UniformRegistry::eraseMaterialUniform(unsigned int matID, const std::string& uniformName) {
    if (!materialUniforms.contains(matID)) return;
    if (!materialUniforms.at(matID).contains(uniformName)) return;
    unsigned int id = materialUniforms.at(matID).at(uniformName);
    if (!project->uniforms.contains(id)) return;
    project->uniforms.erase(id);
    materialUniforms.at(matID).erase(uniformName);

}

void UniformRegistry::registerSceneUniform(Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    sceneUniforms[uniform.name] = uniform.ID;
    project->uniforms[uniform.ID] = uniform;
}


void UniformRegistry::registerModelUniform(unsigned int modelID, Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    modelUniforms[modelID][uniform.name] = uniform.ID;
    project->uniforms[uniform.ID] = uniform;
}



void UniformRegistry::registerMaterialUniform(unsigned int materialID, Uniform uniform) {
    uniform.ID = nextID;
    nextID++;
    materialUniforms[materialID][uniform.name] = uniform.ID;
    project->uniforms[uniform.ID] = uniform;
}

size_t UniformRegistry::getSceneUniformsSize() {
    return sceneUniforms.size();
}
size_t UniformRegistry::getModelUniformsSize(unsigned int modelID) {
    if (!modelUniforms.contains(modelID)) return 0;
    return modelUniforms[modelID].size();
}
size_t UniformRegistry::getMaterialUniformsSize(unsigned int matID) {
    if (!materialUniforms.contains(matID)) return 0;
    return materialUniforms[matID].size();
}

const std::unique_ptr<std::unordered_map<std::string, Uniform>> UniformRegistry::tryReadSceneUniforms() const {
    if (sceneUniforms.size() <= 0) {
        // Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID");
        return nullptr;
    }

    //std::unique_ptr<std::unordered_map<std::string, Uniform>> programUniforms = &uniforms[modelID];
    std::unique_ptr<std::unordered_map<std::string, Uniform>> unis = std::make_unique<std::unordered_map<std::string, Uniform>>();
    for (auto& [name, id] : sceneUniforms) {
        if (!project->uniforms.contains(id)) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "ReadSceneproject->uniforms", "uniform doesn't exist when it should!");
        }
        (*unis.get())[name] = project->uniforms.at(id);
    }

    return unis;
}


const std::unique_ptr<std::unordered_map<std::string, Uniform>> UniformRegistry::tryReadModelUniforms(unsigned int modelID) const {
    if (modelUniforms.count(modelID) <= 0) {
        // Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID", modelID );
        return nullptr;
    }

    //std::unique_ptr<std::unordered_map<std::string, Uniform>> programUniforms = &uniforms[modelID];
    std::unique_ptr<std::unordered_map<std::string, Uniform>> unis = std::make_unique<std::unordered_map<std::string, Uniform>>();
    for (auto& [name, id] : modelUniforms.at(modelID)) {
        if (!project->uniforms.contains(id)) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "ReadSceneproject->uniforms", "uniform doesn't exist when it should!");
        }
        (*unis.get())[name] = project->uniforms.at(id);
    }

    return unis;
}


const std::unique_ptr<std::unordered_map<std::string, Uniform>> UniformRegistry::tryReadMaterialUniforms(unsigned int materialID) const {
    if (materialUniforms.count(materialID) <= 0) {
        // Errorlog::getInstance().logEntry(EL_WARNING, "tryReadUniforms", "No object found in Uniform Registry with ID", modelID );
        return nullptr;
    }

    //std::unique_ptr<std::unordered_map<std::string, Uniform>> programUniforms = &uniforms[modelID];
    std::unique_ptr<std::unordered_map<std::string, Uniform>> unis = std::make_unique<std::unordered_map<std::string, Uniform>>();
    for (auto& [name, id] : (materialUniforms.at(materialID))) {
        if (!project->uniforms.contains(id)) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "ReadSceneproject->uniforms", "uniform doesn't exist when it should!");
        }
        (*unis.get())[name] = project->uniforms.at(id);
    }

    return unis;
}
