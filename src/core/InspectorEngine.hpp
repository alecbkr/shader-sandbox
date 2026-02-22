#pragma once

#include <string>
#include <glm/glm.hpp>
#include "core/ui/InspectorUI.hpp"
#include "core/ui/ViewportUI.hpp"
#include "engine/ShaderProgram.hpp"
#include "core/UniformTypes.hpp"

class Logger;
class ShaderRegistry;
class UniformRegistry;
class ModelCache;

class InspectorEngine {
public:
    InspectorEngine();
    bool initialize(Logger* _loggerPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelCache* _modelCachePtr, ViewportUI* _viewportUIPtr);
    void shutdown();
    void refreshUniforms();
    void applyAllUniformsForObject(unsigned int modelID);
    void setUniform(unsigned int modelID, const std::string& uniformName, UniformValue value);
    void applyInput(unsigned int modelID, const Uniform& uniform);
    void reloadUniforms(unsigned int modelID);
    bool handleEditShaderProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);
    void assignDefaultValue(Uniform& uniform);
    UniformValue getDefaultValue(UniformType type);

private:
    void applyFunction(ShaderProgram& program, const Uniform& uniform, const InspectorReference& function);
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    ViewportUI* viewportUIPtr = nullptr;
    void applyUniform(unsigned int modelID, const Uniform& uniform);
    void applyUniform(ShaderProgram& program, const Uniform& uniform);
};
