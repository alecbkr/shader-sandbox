#include "InspectorEngine.hpp"
#include "core/UniformParser.hpp"
#include "core/logging/LogSink.hpp"
#include "core/ui/ViewportUI.hpp"
#include "engine/Camera.hpp"
#include "engine/Errorlog.hpp"
#include "logging/Logger.hpp"
#include "core/UniformRegistry.hpp"
#include <functional>
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
#include "object/Material.hpp"
#include "object/MaterialCache.hpp"
#include "object/Model.hpp"
#include "engine/Errorlog.hpp"
#include "core/logging/Logger.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "object/ModelCache.hpp"

InspectorEngine::InspectorEngine() {
    loggerPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    materialCachePtr = nullptr;
    modelCachePtr = nullptr;
    initialized = false;
}

bool InspectorEngine::initialize(Logger* _loggerPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelCache* _modelCachePtr, ViewportUI* _viewportUIPtr, MaterialCache* _materialCachePtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Inspector Engine Initialization", "Inspector Engine was already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    shaderRegPtr = _shaderRegPtr;
    uniformRegPtr = _uniformRegPtr;
    viewportUIPtr = _viewportUIPtr;
    materialCachePtr = _materialCachePtr;
    modelCachePtr = _modelCachePtr;

    initialized = true;
    return true;
}

void InspectorEngine::shutdown() {
    loggerPtr = nullptr;
    shaderRegPtr = nullptr;
    uniformRegPtr = nullptr;
    materialCachePtr = nullptr;
    modelCachePtr = nullptr;
    initialized = false;
}

void InspectorEngine::refreshUniforms() {
    const auto& programs = shaderRegPtr->getPrograms();
    
    // NOTE: this will break if we do any multithreading with the program list.
    // Please be careful.
    std::unordered_map<std::string, std::vector<unsigned int>> programToMaterialList;
    for (const auto& [programName, program] : programs) {
        programToMaterialList[programName] = std::vector<unsigned int>();
    }

    const std::vector<unsigned int> materials = materialCachePtr->getAllMaterialIDs();
    for (const unsigned int matID : materials) {
        // separate them so that debugger works.
        Material* mat = materialCachePtr->getMaterial(matID);
        if (mat == nullptr) {
            loggerPtr->addLog(LogLevel::WARNING, "refreshUniforms", "material " + std::to_string(matID) + " does exist!");
        }
        ShaderProgram* matProgram = shaderRegPtr->getProgram(mat->getProgramID());
        if (matProgram != nullptr) {
            programToMaterialList[mat->getProgramID()].push_back(matID);
        }
        else {
            loggerPtr->addLog(LogLevel::WARNING, "refreshUniforms", "material " + std::to_string(matID) + " does not have a shader!");
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
        std::cout << programToMaterialList[programName].size() << std::endl;
        for (unsigned int matID : programToMaterialList[programName]) {
            std::cout << matID << " " << program->name << std::endl;
            const bool newModel = !uniformRegPtr->containsMaterial(matID);
            if (newModel) {
                uniformRegPtr->insertUniformMap(matID, parsedUniforms);
                applyAllUniformsForObject(matID);
                continue;
            }

            // if not a new object
            const auto& objectUniforms = uniformRegPtr->tryReadUniforms(matID);
            if (objectUniforms == nullptr) {
                loggerPtr->addLog(LogLevel::WARNING, "refreshUniforms", "object does not exist in registry??? code should be unreachable");
                continue;
            }

            for (const auto& [uniformName, parsedUniform] : parsedUniforms) {
                const Uniform* existingUniform = uniformRegPtr->tryReadUniform(matID, uniformName);
                const bool mustRegister = existingUniform == nullptr || existingUniform->type != parsedUniform.type;
                
                if (mustRegister) uniformRegPtr->registerInspectorUniform(matID, parsedUniform); 
            }

            std::vector<std::string> uniformsToErase;
            for (const auto& [uniformName, uniform] : *objectUniforms) {
                if (!parsedUniforms.contains(uniformName))       
                    uniformsToErase.push_back(uniformName);
            }

            for (const std::string& uniformName : uniformsToErase) {
                uniformRegPtr->eraseUniform(matID, uniformName);
            }
        }
    }
}

bool InspectorEngine::handleEditShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, const std::string& programName) {
    // Code mostly taken from hotreloader
    ShaderProgram *oldProgram = shaderRegPtr->getProgram(programName);
    
    // Simple path, just register the new program
    if (oldProgram == nullptr) {
        if (!shaderRegPtr->registerProgram(vertexPath, fragmentPath , programName)) return false;
        InspectorEngine::refreshUniforms();
        return true;
    }

    // Otherwise, we need to go through this mess.
    auto newProgram = std::make_unique<ShaderProgram>(vertexPath.c_str(), fragmentPath.c_str(), programName.c_str(), loggerPtr);
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
    case UniformType::SamplerCube:
        // uniform.value = InspectorSamplerCube{.textureUnit = 0}; // Default to texture unit 0
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

void InspectorEngine::setUniform(unsigned int materialID, const std::string& uniformName, UniformValue value) {
    const Uniform* const oldUniform = uniformRegPtr->tryReadUniform(materialID, uniformName);
    if (oldUniform != nullptr) {
        Uniform newUniform = *oldUniform;
        newUniform.value = value;
        uniformRegPtr->registerInspectorUniform(materialID, newUniform);

        applyUniform(materialID, newUniform);
    }
    else {
        loggerPtr->addLog(LogLevel::WARNING, "setUniform", "failed to set:", uniformName.c_str()); 
    }
}

void InspectorEngine::applyAllUniformsForObject(unsigned int materialID) {
    const auto objectUniforms = uniformRegPtr->tryReadUniforms(materialID);

    if (objectUniforms == nullptr) {
        // ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", modelID.c_str());
        loggerPtr->addLog(LogLevel::WARNING, "applyAllUniformsForObject", "material not found in uniform registry: ", std::to_string(materialID)); 
        return;
    }

    for (auto& [uniformName, uniform] : *objectUniforms) {
        applyUniform(materialID, uniform);
    }
}

void InspectorEngine::applyUniform(unsigned int materialID, const Uniform& uniform) {
    if (!materialCachePtr->contains(materialID)) {
        // ERRLOG.logEntry(EL_WARNING, "applyUniform", (modelID + " not found in registry").c_str());
        loggerPtr->addLog(LogLevel::WARNING, "applyUniform", std::to_string(materialID) + " not found in registry"); 
        return;
    }
    Material* mat = materialCachePtr->getMaterial(materialID);
    if (mat == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform", "material " + std::to_string(materialID) + " doesn't exist!");
        return;
    }
    ShaderProgram* matProgram = shaderRegPtr->getProgram(mat->getProgramID());
    if (matProgram == nullptr) {
        loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform", "model " + std::to_string(materialID) + " has no shader program!");
        return;
    }

    applyUniform(*matProgram, uniform);
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
    case UniformType::SamplerCube: {
        break; // don't do anything yet;
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
    std::unordered_set<unsigned int> matIDs;
    InspectorReference currentFunction = function;
    Uniform currentUniform = uniform;
    Uniform finalValue;

    while (currentUniform.isFunction) {
        matIDs.insert(currentUniform.materialID);
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
                loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", "referenced Model does not exist!");
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

        if (matIDs.contains(currentFunction.referencedMaterialID)) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyFunction", 
                        "function references same model (ID " + std::to_string(currentFunction.referencedMaterialID) + "), would create circular reference");
            break;
        }

        const Uniform* referencedUniform = uniformRegPtr->tryReadUniform(currentFunction.referencedMaterialID, currentFunction.referencedUniformName); 
        if (referencedUniform == nullptr) {
            loggerPtr->addLog(LogLevel::LOG_ERROR, "applyUniform: Function, ", "referenced uniform for " + std::to_string(currentFunction.referencedMaterialID) + ": " + currentFunction.referencedUniformName + "does not exist!");
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
void InspectorEngine::applyInput(unsigned int matID, const Uniform& uniform) {
    uniformRegPtr->registerInspectorUniform(matID, uniform);
    applyUniform(matID, uniform);
}
void InspectorEngine::reloadUniforms(unsigned int materialID) {
    Material* targetMat = materialCachePtr->getMaterial(materialID);

    if (targetMat == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "reloadUniforms", "material " + std::to_string(materialID) + " does not exist!");
        return;
    }
    ShaderProgram* matProgram = shaderRegPtr->getProgram(targetMat->getProgramID());
    if (matProgram == nullptr || !matProgram->isCompiled()) {
        loggerPtr->addLog(LogLevel::WARNING, "reloadUniforms", "material " + std::to_string(materialID) + " has no shader! or is not compiled");
        return;
    }

    UniformParser parser(loggerPtr);
    auto newUniforms = parser.parseUniforms(*matProgram);
    for (auto& [name, uniform] : newUniforms) {
        uniform.value = getDefaultValue(uniform.type);
    }

    for (const unsigned int matID : materialCachePtr->getAllMaterialIDs()) {
        Material* matPtr = materialCachePtr->getMaterial(matID);
        if (matPtr && matPtr->getProgramID() == matProgram->name) {
            matPtr->setProgramID(matProgram->name); 
            const auto existingRegistry = uniformRegPtr->tryReadUniforms(matID);
            if (existingRegistry) {
                for (auto& [uName, uData] : newUniforms) {
                    if (existingRegistry->contains(uName)) {
                        uData.value = existingRegistry->at(uName).value;
                    }
                }
            }

            uniformRegPtr->insertUniformMap(matID, newUniforms);
            matProgram->use();
            applyAllUniformsForObject(matID);
        }
    }
}


void InspectorEngine::applyAllUniformsForPrimitive(ModelPrimitive prim) {
    Material* mat = materialCachePtr->getMaterial(prim.materialID);
    if (mat == nullptr) {
        loggerPtr->addLog(LogLevel::WARNING, "reloadUniforms", "material " + std::to_string(prim.materialID) + " does not exist!");
        return;
    }
    ShaderProgram* matProgram = shaderRegPtr->getProgram(mat->getProgramID());
    if (matProgram == nullptr || !matProgram->isCompiled()) {
        loggerPtr->addLog(LogLevel::WARNING, "reloadUniforms", "material " + std::to_string(prim.materialID) + " has no shader! or is not compiled");
        return;
    }
    matProgram->use();

    // temp fix cause we have a meeting in 2 hours
    // I need to refactor the uniform registry cause it's current state is a mess
    const auto uniforms = uniformRegPtr->tryReadUniforms(prim.materialID);
    if (uniforms == nullptr) {
        // ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", modelID.c_str());
        // Logger::addLog(LogLevel::WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", std::to_string(modelID)); 
        return;
    }

    for (auto& [uniformName, uniform] : *uniforms) {
        applyUniform(*matProgram, uniform);
    }

    applySceneUniforms(*matProgram);
    applyModelUniforms(*matProgram, prim.ModelID);
    applyMaterialUniforms(*matProgram, prim.ModelID, prim.materialID);

}


void InspectorEngine::applySceneUniforms(ShaderProgram& program) {
    const auto sceneUniforms = uniformRegPtr->tryReadSceneUniforms();
    if (sceneUniforms == nullptr) {
        // ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", modelID.c_str());
        // Logger::addLog(LogLevel::WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", std::to_string(modelID)); 
        return;
    }

    for (auto& [uniformName, uniform] : *sceneUniforms) {
        applyUniform(program, uniform);
    }
}


void InspectorEngine::applyModelUniforms(ShaderProgram& program, unsigned int modelID) {
    const auto modelUniforms = uniformRegPtr->tryReadModelUniforms(modelID);
    if (modelUniforms == nullptr) {
        // ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", modelID.c_str());
        // Logger::addLog(LogLevel::WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", std::to_string(modelID)); 
        return;
    }

    for (auto& [uniformName, uniform] : *modelUniforms) {
        applyUniform(program, uniform);
    }
}


void InspectorEngine::applyMaterialUniforms(ShaderProgram& program, unsigned int modelID, unsigned int materialID) {
    const auto materialUniforms = uniformRegPtr->tryReadMaterialUniforms(materialID);
    if (materialUniforms == nullptr) {
        // ERRLOG.logEntry(EL_WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", modelID.c_str());
        // Logger::addLog(LogLevel::WARNING, "applyAllUniformsForObject", "object not found in uniform registry: ", std::to_string(modelID)); 
        return;
    }

    for (auto& [uniformName, uniform] : *materialUniforms) {
        applyUniform(program, uniform);
    }
}



