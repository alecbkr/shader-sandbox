#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include "engine/ShaderProgram.hpp"
#include "core/UniformTypes.hpp"

class Logger;
class ShaderRegistry;
class UniformRegistry;
class ModelCache;

class InspectorEngine {
public:
    InspectorEngine();
    bool initialize(Logger* _loggerPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelCache* _modelCachePtr);
    void shutdown();
    void refreshUniforms();
    void applyAllUniformsForObject(unsigned int modelID);
    void setUniform(unsigned int modelID, const std::string& uniformName, UniformValue value);
    void applyInput(unsigned int modelID, const Uniform& uniform);
    void reloadUniforms(unsigned int modelID);

private:
    static const std::unordered_map<std::string, UniformType> typeMap; // Kept private
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    void applyUniform(unsigned int modelID, const Uniform& uniform);
    void applyUniform(ShaderProgram& program, const Uniform& uniform);
    std::unordered_map<std::string, Uniform> parseUniforms(const ShaderProgram& program);
    void assignDefaultValue(Uniform& uniform);
};