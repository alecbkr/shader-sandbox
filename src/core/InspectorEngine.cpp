#include "InspectorEngine.hpp"
#include "core/UniformRegistry.hpp"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "core/ShaderRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/Model.hpp"
#include "engine/Errorlog.hpp"

const std::unordered_map<std::string, UniformType> InspectorEngine::typeMap = {
    {"vec3", UniformType::Vec3},
    {"vec4", UniformType::Vec4},
    {"int", UniformType::Int},
    {"float", UniformType::Float},
    {"mat4", UniformType::Mat4 },
    {"sampler2D", UniformType::Int }
};

InspectorEngine::InspectorEngine() {
    initialized = false;
    loggerPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    modelCachePtr = nullptr;
}

bool InspectorEngine::initialize(Logger* _loggerPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelCache* _modelCachePtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Inspector Engine Initialization", "Inspector Engine was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    modelCachePtr = _modelCachePtr;
    
    refreshUniforms();

    initialized = true;
    return true;
}

void InspectorEngine::shutdown() {
    loggerPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    modelCachePtr = nullptr;
    initialized = false;
}

void InspectorEngine::refreshUniforms() {
    auto& programs = shaderRegPtr->getPrograms();
    
    // NOTE: this will break if we do any multithreading with the program list.
    // Please be careful.
    std::unordered_map<std::string, std::vector<unsigned int>> programToObjectList;
    for (const auto& [programName, program] : programs) {
        programToObjectList[programName] = std::vector<unsigned int>();
    }

    for (const auto& pair : modelCachePtr->modelIDMap) {
        // separate them so that debugger works.
        const auto& modelID = pair.first;
        const auto& model = pair.second;
        if (model->getProgram() != nullptr) {
            const auto& programName = model->getProgram()->name;
            programToObjectList[programName].push_back(modelID);
        }
    }

    // sry for the nesting
    for (const auto& [programName, program] : programs) {
        const auto& parsedUniforms = parseUniforms(*program);
        std::cout << programName << std::endl;
        std::cout << programToObjectList[programName].size() << std::endl;
        for (unsigned int modelID : programToObjectList[programName]) {
            std::cout << modelID << " " << program->name << std::endl;
            const bool newModel = !UNIFORM_REGISTRY.containsObject(modelID);
            if (newModel) {
                UNIFORM_REGISTRY.insertUniformMap(modelID, parsedUniforms);
                applyAllUniformsForObject(modelID);
                continue;
            }

            // if not a new object
            const auto& objectUniforms = UNIFORM_REGISTRY.tryReadUniforms(modelID);
            if (objectUniforms == nullptr) {
                ERRLOG.logEntry(EL_WARNING, "refreshUniforms", "object does not exist in registry??? code should be unreachable");
                continue;
            }

            for (const auto& [uniformName, parsedUniform] : parsedUniforms) {
                const Uniform* existingUniform = UNIFORM_REGISTRY.tryReadUniform(modelID, uniformName);
                const bool mustRegister = existingUniform == nullptr || existingUniform->type != parsedUniform.type;
                
                if (mustRegister) UNIFORM_REGISTRY.registerUniform(modelID, parsedUniform); 
            }

            std::vector<std::string> uniformsToErase;
            for (const auto& [uniformName, uniform] : *objectUniforms) {
                if (!parsedUniforms.contains(uniformName))       
                    uniformsToErase.push_back(uniformName);
            }

            for (const std::string& uniformName : uniformsToErase) {
                UNIFORM_REGISTRY.eraseUniform(modelID, uniformName);
            }
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
                // ERRLOG.logEntry(EL_WARNING, "parseUniforms", "Invalid Uniform Type: ", word.c_str());
                loggerPtr->addLog(LogLevel::WARNING, "parseUnifroms", "Invalid Uniform Type: ", word); 
                continue;
            }
            assignDefaultValue(uniform);

            // Assign the uniform name
            line_ss >> uniform.name; // third word of the line (uniform's name)
            if (!uniform.name.empty() && uniform.name.back() == ';') {
                uniform.name.pop_back();
            }

            uniform.ref.shaderName = program.name;
            programUniforms[uniform.name] = uniform;
            std::cout << "Read " << uniform.name << std::endl;
        }
    }

    // I'm really not sure this is necessary, it's a bit of a relic from the prototype where the uniform registry wasn't the source of truth...
    /*
    // Find old program uniforms
    auto oldProgramPair = uniforms.find(program->name);
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
    case UniformType::Mat4:
        uniform.value = glm::mat4(0.0f);
        break;
    default:
        // ERRLOG.logEntry(EL_WARNING, "assignDefaultValue", "Invalid Uniform Type, making it an int");
        loggerPtr->addLog(LogLevel::WARNING, "assignDefaultValue", "Invalid Uniform Type, making it an int"); 
        uniform.type = UniformType::Int;
        uniform.value = 0;
        break;
    }
}

void InspectorEngine::setUniform(unsigned int modelID, const std::string& uniformName, UniformValue value) {
    const Uniform* const oldUniform = UNIFORM_REGISTRY.tryReadUniform(modelID, uniformName);
    if (oldUniform != nullptr) {
        Uniform newUniform = *oldUniform;
        newUniform.value = value;
        UNIFORM_REGISTRY.registerUniform(modelID, newUniform);

        applyUniform(modelID, newUniform);
    }
    else {
        // ERRLOG.logEntry(EL_WARNING, "setUniform", "failed to set: ", uniformName.c_str());
        loggerPtr->addLog(LogLevel::WARNING, "setUniform", "failed to set:", uniformName.c_str()); 
    }
}

void InspectorEngine::applyAllUniformsForObject(unsigned int modelID) {
    const std::unordered_map<std::string, Uniform>* objectUniforms = UNIFORM_REGISTRY.tryReadUniforms(modelID);

    if (objectUniforms == nullptr) {
        // ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", modelID.c_str());
        loggerPtr->addLog(LogLevel::WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", std::to_string(modelID)); 
        return;
    }

    for (auto& [uniformName, uniform] : *objectUniforms) {
        applyUniform(modelID, uniform);
    }
}

void InspectorEngine::applyUniform(unsigned int modelID, const Uniform& uniform) {
    if (!modelCachePtr->modelIDMap.contains(modelID)) {
        // ERRLOG.logEntry(EL_WARNING, "applyUniform", (modelID + " not found in registry").c_str());
        loggerPtr->addLog(LogLevel::WARNING, "applyUniform", (modelID + " not found in registry")); 
        return;
    }
    Model& model = *modelCachePtr->modelIDMap.at(modelID);
    ShaderProgram* program = shaderRegPtr->getProgram(model.getProgram()->name);
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
    case UniformType::Mat4:
        program.setUniform_mat4float(uniform.name.c_str(), std::get<glm::mat4>(uniform.value));
        break;
    default:
        // ERRLOG.logEntry(EL_WARNING, "applyUniform", "Invalid Uniform Type: ");
        loggerPtr->addLog(LogLevel::WARNING, "applyUniform", "Invalid Uniform Type: "); 
        break;
    }
}

// Include this along with setUniform because setUniform is used for other stuff.
void InspectorEngine::applyInput(unsigned int modelID, const Uniform& uniform) {
    UNIFORM_REGISTRY.registerUniform(modelID, uniform);
    applyUniform(modelID, uniform);
}
void InspectorEngine::reloadUniforms(unsigned int modelID) {
    Model* targetModel = modelCachePtr->getModel(modelID);
    if (!targetModel || !targetModel->getProgram()) return;

    std::string programName = targetModel->getProgram()->name;
    ShaderProgram* newProgram = shaderRegPtr->getProgram(programName);
    
    if (newProgram == nullptr || !newProgram->isCompiled()) return;

    auto newUniforms = parseUniforms(*newProgram);

    for (auto& [id, modelPtr] : modelCachePtr->modelIDMap) {
        if (modelPtr && modelPtr->getProgram() && modelPtr->getProgram()->name == programName) {
            modelPtr->setProgram(*newProgram); 
            const auto* existingRegistry = UNIFORM_REGISTRY.tryReadUniforms(id);
            if (existingRegistry) {
                for (auto& [uName, uData] : newUniforms) {
                    if (existingRegistry->contains(uName)) {
                        uData.value = existingRegistry->at(uName).value;
                    }
                }
            }

            UNIFORM_REGISTRY.insertUniformMap(id, newUniforms);
            newProgram->use();
            applyAllUniformsForObject(id);
        }
    }
}
