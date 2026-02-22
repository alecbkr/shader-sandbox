#include "InspectorEngine.hpp"
#include "core/UniformParser.hpp"
#include "core/logging/LogSink.hpp"
#include "core/ui/ViewportUI.hpp"
#include "engine/Camera.hpp"
#include "engine/Errorlog.hpp"
#include "logging/Logger.hpp"
#include "core/UniformRegistry.hpp"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include "core/UniformTypes.hpp"
#include "core/logging/Logger.hpp"
#include "engine/ShaderProgram.hpp"
#include "object/Model.hpp"
#include "engine/Errorlog.hpp"
#include "core/logging/Logger.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "object/ModelCache.hpp"

InspectorEngine::InspectorEngine() {
    initialized = false;
    loggerPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    modelCachePtr = nullptr;
}

bool InspectorEngine::initialize(Logger* _loggerPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelCache* _modelCachePtr, ViewportUI* _viewportUIPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Inspector Engine Initialization", "Inspector Engine was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    modelCachePtr = _modelCachePtr;
    viewportUIPtr = _viewportUIPtr;
    
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
    const auto& programs = shaderRegPtr->getPrograms();
    
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
        ShaderProgram* modelProgram = shaderRegPtr->getProgram(model->getProgramID());
        if (modelProgram != nullptr) {
            programToObjectList[model->getProgramID()].push_back(modelID);
        }
        else {
            loggerPtr->addLog(LogLevel::WARNING, "refreshUniforms", "object " + std::to_string(modelID) + " does not have a shader!");
        }
    }

    // sry for the nesting
    UniformParser parser(loggerPtr);

    for (const auto& [programName, program] : programs) {
        auto parsedUniforms = parser.parseUniforms(*program);
        for (auto& [name, uniform] : parsedUniforms) {
            uniform.value = getDefaultValue(uniform.type);
        }
        std::cout << programName << std::endl;
        std::cout << programToObjectList[programName].size() << std::endl;
        for (unsigned int modelID : programToObjectList[programName]) {
            std::cout << modelID << " " << program->name << std::endl;
            const bool newModel = !uniformRegPtr->containsObject(modelID);
            if (newModel) {
                uniformRegPtr->insertUniformMap(modelID, parsedUniforms);
                applyAllUniformsForObject(modelID);
                continue;
            }

            // if not a new object
            const auto& objectUniforms = uniformRegPtr->tryReadUniforms(modelID);
            if (objectUniforms == nullptr) {
                loggerPtr->addLog(LogLevel::WARNING, "refreshUniforms", "object does not exist in registry??? code should be unreachable");
                continue;
            }

            for (const auto& [uniformName, parsedUniform] : parsedUniforms) {
                const Uniform* existingUniform = uniformRegPtr->tryReadUniform(modelID, uniformName);
                const bool mustRegister = existingUniform == nullptr || existingUniform->type != parsedUniform.type;
                
                if (mustRegister) uniformRegPtr->registerUniform(modelID, parsedUniform); 
            }

            std::vector<std::string> uniformsToErase;
            for (const auto& [uniformName, uniform] : *objectUniforms) {
                if (!parsedUniforms.contains(uniformName))       
                    uniformsToErase.push_back(uniformName);
            }

            for (const std::string& uniformName : uniformsToErase) {
                uniformRegPtr->eraseUniform(modelID, uniformName);
            }
        }
    }
}

bool InspectorEngine::handleEditShaderProgram(const std::string& vertexFile, const std::string& fragmentFile, const std::string& programName) {
    // Code mostly taken from hotreloader
    ShaderProgram *oldProgram = shaderRegPtr->getProgram(programName);
    
    // Simple path, just register the new program
    if (oldProgram == nullptr) {
        if (!shaderRegPtr->registerProgram(vertexFile, fragmentFile , programName)) return false;
        InspectorEngine::refreshUniforms();
        return true;
    }

    // Otherwise, we need to go through this mess.
    auto newProgram = std::make_unique<ShaderProgram>(vertexFile.c_str(), fragmentFile.c_str(), programName.c_str(), loggerPtr);
    if (!newProgram->isCompiled()) return false;

    shaderRegPtr->replaceProgram(programName, std::move(newProgram));

    InspectorEngine::refreshUniforms();
    return true;
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
    case UniformType::Sampler2D:
        uniform.value = InspectorSampler2D{.textureUnit = 0}; // Default to texture unit 0
        break;
    default:
        loggerPtr->addLog(LogLevel::WARNING, "assignDefaultValue", "Invalid Uniform Type, making it an int"); 
        uniform.type = UniformType::Int;
        uniform.value = 0;
        break;
    }
}

UniformValue InspectorEngine::getDefaultValue(UniformType type) {
    switch (type) {
    case UniformType::Int:
        return 0;
        break;
    case UniformType::Float:
        return 0.0f;
        break;
    case UniformType::Vec3:
        return glm::vec3(0.0f);
        break;
    case UniformType::Vec4:
        return glm::vec4(0.0f);
        break;
    case UniformType::Mat4:
        return glm::mat4(0.0f);
        break;
    case UniformType::Sampler2D:
        return InspectorSampler2D{.textureUnit = 0}; // Default to texture unit 0
        break;
    default:
        // Logger::addLog(LogLevel::WARNING, "assignDefaultValue", "Invalid Uniform Type, making it an int");
        loggerPtr->addLog(LogLevel::WARNING, "getDefaultValue", "Invalid Uniform Type, making it an int"); 
        return 0;
        break;
    }
}

void InspectorEngine::setUniform(unsigned int modelID, const std::string& uniformName, UniformValue value) {
    const Uniform* const oldUniform = uniformRegPtr->tryReadUniform(modelID, uniformName);
    if (oldUniform != nullptr) {
        Uniform newUniform = *oldUniform;
        newUniform.value = value;
        uniformRegPtr->registerUniform(modelID, newUniform);

        applyUniform(modelID, newUniform);
    }
    else {
        loggerPtr->addLog(LogLevel::WARNING, "setUniform", "failed to set:", uniformName.c_str()); 
    }
}

void InspectorEngine::applyAllUniformsForObject(unsigned int modelID) {
    const std::unordered_map<std::string, Uniform>* objectUniforms = uniformRegPtr->tryReadUniforms(modelID);

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
        loggerPtr->addLog(LogLevel::WARNING, "applyUniform", std::to_string(modelID) + " not found in registry"); 
        return;
    }
    Model& model = *modelCachePtr->modelIDMap.at(modelID);
    ShaderProgram* modelProgram = shaderRegPtr->getProgram(model.getProgramID());
    if (modelProgram == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform", "model " + std::to_string(modelID) + " has no shader program!");
        return;
    }

    applyUniform(*modelProgram, uniform);
}

void InspectorEngine::applyUniform(ShaderProgram& program, const Uniform& uniform) {
    program.use();

    // Get info from the reference and then apply it to this uniform.
    // Add input validation here!!!
    // Also, at some point, we're going to want a memo so we don't do link list traversal if things haven't changed
    if (uniform.isFunction) {

        const InspectorReference* function = std::get_if<InspectorReference>(&uniform.value);
        if (function == nullptr) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform", "isFunction is set but uniform: " + uniform.name + " does not have a function value!");
            return;
        }
        applyFunction(program, uniform, *function);
        return;
    }

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
    case UniformType::Sampler2D: {
        break; // don't do anything yet
        const InspectorSampler2D& sampler = std::get<InspectorSampler2D>(uniform.value);
        program.setUniform_int(uniform.name.c_str(), sampler.textureUnit);
        break;
    }
    default:
        loggerPtr->addLog(LogLevel::WARNING, "applyUniform", "Invalid Uniform Type: "); 
        break;
    }
    return;
}

void InspectorEngine::applyFunction(ShaderProgram& program, const Uniform& uniform, const InspectorReference& function) {
    // Going to have to rework this once we get more functions
    // If at any point we run into an invalid function, we should use a default value as to not break the rest of the program.
    bool validFunction = false;

    // Traverse function like a graph. Will need to implement some kind of DFS or something eventually to avoid slowness
    std::unordered_set<unsigned int> modelIDs;
    InspectorReference currentFunction = function;
    Uniform currentUniform = uniform;
    Uniform finalValue;

    while (currentUniform.isFunction) {
        modelIDs.insert(currentUniform.modelID);
        if (!currentFunction.initialized) {
            break;
        }

        if (currentFunction.returnType != currentUniform.type) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", 
                        "return type mismatch: function returns " + to_string(currentFunction.returnType) 
                        + ", uniform expects " + to_string(currentUniform.type));
            break;
        }

        if (currentFunction.useWorldData) {
            // need a better way of doing this...
            if (currentFunction.useCamaraData) {
                switch (currentFunction.returnType) {
                case UniformType::Vec3: {
                    const Camera* const cam = viewportUIPtr->getCamera();
                    if (cam == nullptr) {
                        loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", "camera is null!");
                        continue;
                    }
                    if (function.referencedUniformName == "position") {
                        finalValue = uniform;
                        finalValue.name = uniform.name;
                        finalValue.isFunction = false;
                        finalValue.value = cam->Position;
                        validFunction = true;
                    }
                    break;
                }
                case UniformType::Vec4: {
                    break;
                }
                default: 
                    loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", "This type " + to_string(function.returnType) + " does not support world Data!");
                    break;
                }
                break;
            }
            Model* referencedModel = modelCachePtr->getModel(function.referencedModelID);
            if (referencedModel == nullptr) {
                loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", "referenced model does not exist!");
                break;
            }

            switch (function.returnType) {
            case UniformType::Vec3: {
                if (function.referencedUniformName == "position") {
                    finalValue = uniform;
                    finalValue.name = uniform.name;
                    finalValue.isFunction = false;
                    finalValue.value = referencedModel->getPosition();
                    validFunction = true;
                }
                else if (function.referencedUniformName == "scale") {
                    finalValue = uniform;
                    finalValue.name = uniform.name;
                    finalValue.isFunction = false;
                    finalValue.value = referencedModel->getScale();
                    validFunction = true;
                }
                break;
            }
            case UniformType::Vec4: {
                if (function.referencedUniformName == "orientation") {
                    finalValue = uniform;
                    finalValue.name = uniform.name;
                    finalValue.isFunction = false;
                    finalValue.value = referencedModel->getRotation();
                    validFunction = true;
                }
                break;
            }
            default: 
                loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", "This type " + to_string(function.returnType) + " does not support world Data!");
                break;
            }

            break;
        }

        if (modelIDs.contains(currentFunction.referencedModelID)) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", 
                        "function references same model (ID " + std::to_string(currentFunction.referencedModelID) + "), would create circular reference");
            break;
        }

        const Uniform* referencedUniform = uniformRegPtr->tryReadUniform(currentFunction.referencedModelID, currentFunction.referencedUniformName); 
        if (referencedUniform == nullptr) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform: Function, ", "referenced uniform for " + std::to_string(currentFunction.referencedModelID) + ": " + currentFunction.referencedUniformName + "does not exist!");
            break;
        }

        // This is the only successful path
        if (!referencedUniform->isFunction) {
            if (referencedUniform->type != currentFunction.returnType) {
                loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", 
                            "reference type mismatch: referenced uniform has type " + to_string(referencedUniform->type) 
                            + ", uniform expects " + to_string(currentUniform.type));
                break;
            }
            finalValue = *referencedUniform;
            finalValue.name = uniform.name;

            validFunction = true;
            break;
        }

        const InspectorReference* referencedFunction = std::get_if<InspectorReference>(&referencedUniform->value);
        if (referencedFunction == nullptr) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform", "isFunction is set but uniform: " + uniform.name + " does not have a function value!");
            break;
        }

        currentFunction = *referencedFunction;
        currentUniform = *referencedUniform;
    
    }

    // in case of an invalid function, we need to just assign it a default value not to break things.
    if (!validFunction) {
        finalValue = uniform;
        finalValue.isFunction = false;
        finalValue.type = function.returnType;
        finalValue.value = getDefaultValue(finalValue.type);
    }

    applyUniform(program, finalValue);
}

// Include this along with setUniform because setUniform is used for other stuff.
void InspectorEngine::applyInput(unsigned int modelID, const Uniform& uniform) {
    uniformRegPtr->registerUniform(modelID, uniform);
    applyUniform(modelID, uniform);
}
void InspectorEngine::reloadUniforms(unsigned int modelID) {
    Model* targetModel = modelCachePtr->getModel(modelID);

    if (targetModel == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "reloadUniforms", "model " + std::to_string(modelID) + " does not exist!");
        return;
    }
    ShaderProgram* modelProgram = shaderRegPtr->getProgram(targetModel->getProgramID());
    if (modelProgram == nullptr || !modelProgram->isCompiled()) {
        loggerPtr->addLog(LogLevel::WARNING, "reloadUniforms", "model " + std::to_string(modelID) + " has no shader! or is not compiled");
        return;
    }

    UniformParser parser(loggerPtr);
    auto newUniforms = parser.parseUniforms(*modelProgram);
    for (auto& [name, uniform] : newUniforms) {
        uniform.value = getDefaultValue(uniform.type);
    }

    for (auto& [id, modelPtr] : modelCachePtr->modelIDMap) {
        if (modelPtr && modelPtr->getProgramID() == modelProgram->name) {
            modelPtr->setProgramID(modelProgram->name); 
            const auto* existingRegistry = uniformRegPtr->tryReadUniforms(id);
            if (existingRegistry) {
                for (auto& [uName, uData] : newUniforms) {
                    if (existingRegistry->contains(uName)) {
                        uData.value = existingRegistry->at(uName).value;
                    }
                }
            }

            uniformRegPtr->insertUniformMap(id, newUniforms);
            modelProgram->use();
            applyAllUniformsForObject(id);
        }
    }
}
