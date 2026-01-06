#include "InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include "../engine/Errorlog.hpp"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "core/ShaderHandler.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/ObjCache.hpp"
#include "object/Object.hpp"
#include "engine/Errorlog.hpp"

const std::unordered_map<std::string, UniformType> InspectorEngine::typeMap = {
    {"vec3", UniformType::Vec3},
    {"vec4", UniformType::Vec4},
    {"int", UniformType::Int},
    {"float", UniformType::Float}
};

InspectorEngine::InspectorEngine(): uniformRegistry(UniformRegistry::instance()) {
    std::cout << "Initializing Inspector Engine" << std::endl;
    refreshUniforms();
}

void InspectorEngine::refreshUniforms() {
    auto& programs = ShaderHandler::getPrograms();
    
    // NOTE: this will break if we do any multithreading with the program list.
    // Please be careful.
    std::unordered_map<std::string, std::vector<std::string>> programToObjectList;
    for (auto& [programName, program] : programs) {
        programToObjectList[programName] = std::vector<std::string>();
    }

    for (auto& pair : ObjCache::objMap) {
        // separate them so that debugger works.
        auto& objectName = pair.first;
        auto& object = pair.second;
        auto& programName = object->getProgram()->name;
        programToObjectList[programName].push_back(objectName);
    }

    for (auto& [programName, program] : programs) {
        auto uniformMap = parseUniforms(program);
        std::cout << programName << std::endl;
        std::cout << programToObjectList[programName].size() << std::endl;
        for (std::string& objectName : programToObjectList[programName]) {
            std::cout << objectName << " " << program.name << std::endl;
            uniformRegistry.insertUniformMap(objectName, uniformMap);
            applyAllUniformsForObject(objectName);
        }
    }
}

std::unordered_map<std::string, Uniform> InspectorEngine::parseUniforms(const ShaderProgram& program) {
    std::unordered_map<std::string, Uniform> programUniforms;

    std::string line;
    for (size_t i = 0; i < 2; i++) {
        std::stringstream sourceCode = i == 0 ?
            std::stringstream(program.vertShader_code) :
            std::stringstream(program.fragShader_code);
        
        while (std::getline(sourceCode, line)) {
            // Figure out if line is a uniform line
            std::istringstream line_ss(line);
            std::string word;
            line_ss >> word; // first word of the line (test for "uniform")

            bool isUniformLine = word == "uniform";
            if (!isUniformLine) continue; // not a uniform line

            // Determine the type of uniform
            Uniform uniform;
            line_ss >> word; // second word of the line (look for type of uniform)

            auto typePair = typeMap.find(word);
            if (typePair != typeMap.end()) {
                uniform.type = typePair->second;
            } else {
                ERRLOG.logEntry(EL_WARNING, "parseUniforms", "Invalid Uniform Type: ", word.c_str());
                continue;
            }
            assignDefaultValue(uniform);

            // Assign the uniform name
            line_ss >> uniform.name; // third word of the line (uniform's name)
            if (!uniform.name.empty() && uniform.name.back() == ';') {
                uniform.name.pop_back();
            }

            programUniforms[uniform.name] = uniform;
            std::cout << "Read " << uniform.name << std::endl;
        }
    }

    // I'm really not sure this is necessary, it's a bit of a relic from the prototype where the uniform registry wasn't the source of truth...
    /*
    // Find old program uniforms
    auto oldProgramPair = uniforms.find(program.name);
    if (oldProgramPair != uniforms.end()) {
        auto& oldProgramUniforms = oldProgramPair->second;
        
        // Loop through every new uniform
        for (auto& [uniformName, uniform] : programUniforms) {
            // Try to find a matching old uniform
            auto oldUniformPair = oldProgramUniforms.find(uniformName);
            if (oldUniformPair == oldProgramUniforms.end()) continue;
            const Uniform& oldUniform = oldUniformPair->second;

            // If old uniform type matches the new type, use old value
            if (uniform.type == oldUniform.type){
                uniform.value = oldUniform.value;
            }
        }
    }

    */
    return programUniforms;
}

void InspectorEngine::assignDefaultValue(Uniform& uniform) {
    switch (uniform.type) {
    case UniformType::Int:
        uniform.value = 0;
        break;
    case UniformType::Float:
        uniform.value = 0.0f;
        break;
    case UniformType::Vec3:
        uniform.value = glm::vec3(0.0f);
        break;
    case UniformType::Vec4:
        uniform.value = glm::vec4(0.0f);
        break;
    default:
        ERRLOG.logEntry(EL_WARNING, "assignDefaultValue", "Invalid Uniform Type, making it an int");
        uniform.type = UniformType::Int;
        uniform.value = 0;
        break;
    }
}

void InspectorEngine::setUniform(const std::string& objectName, const std::string& uniformName, UniformValue value) {
    const Uniform* const oldUniform = uniformRegistry.tryReadUniform(objectName, uniformName);
    if (oldUniform != nullptr) {
        Uniform newUniform = *oldUniform;
        newUniform.value = value;
        uniformRegistry.registerUniform(objectName, newUniform);

        applyUniform(objectName, newUniform);
    }
    else {
        ERRLOG.logEntry(EL_WARNING, "setUniform", "failed to set: ", uniformName.c_str());
    }
}

void InspectorEngine::applyAllUniformsForObject(const std::string& objectName) {
    const std::unordered_map<std::string, Uniform>* objectUniforms = uniformRegistry.tryReadUniforms(objectName);

    if (objectUniforms == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForProgram", "object not found in uniform registry: ", objectName.c_str());
    }

    for (auto& [uniformName, uniform] : *objectUniforms) {
        applyUniform(objectName, uniform);
    }
}

void InspectorEngine::applyUniform(const std::string& objectName, const Uniform& uniform) {
    if (!ObjCache::objMap.contains(objectName)) {
        ERRLOG.logEntry(EL_WARNING, "applyUniform", (objectName + " not found in registry").c_str());
        return;
    }
    Object& object = *ObjCache::objMap.at(objectName);
    ShaderProgram* program = ShaderHandler::getProgram(object.getProgram()->name);
    applyUniform(*program, uniform);
}

void InspectorEngine::applyUniform(ShaderProgram& program, const Uniform& uniform) {
    program.use();
    switch (uniform.type)
    {
    case UniformType::Int:
        program.setUniform_int(uniform.name.c_str(), std::get<int>(uniform.value));
        break;

    case UniformType::Float:
        program.setUniform_float(uniform.name.c_str(), std::get<float>(uniform.value));
        break;

    case UniformType::Vec3:
        program.setUniform_vec3float(uniform.name.c_str(), std::get<glm::vec3>(uniform.value));
        break;

    case UniformType::Vec4:
        program.setUniform_vec4float(uniform.name.c_str(), std::get<glm::vec4>(uniform.value));
        break;
    
    default:
        ERRLOG.logEntry(EL_WARNING, "applyUniform", "Invalid Uniform Type: ");
        break;
    }
}

// Include this along with setUniform because setUniform is used for other stuff.
void InspectorEngine::applyInput(const std::string& objectName, const Uniform& uniform) {
    uniformRegistry.registerUniform(objectName, uniform);
    applyUniform(objectName, uniform);
}
