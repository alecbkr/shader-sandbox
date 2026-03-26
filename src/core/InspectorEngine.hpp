#pragma once

#include <string>
#include <glm/glm.hpp>
#include "UniformTypes.hpp"
#include "core/ui/ViewportUI.hpp"

class Logger;
class ShaderRegistry;
class ShaderProgram;
class UniformRegistry;
class ModelCache;
class MaterialCache;
class ViewportUI;

class InspectorEngine {
public:
    InspectorEngine();
    bool initialize(Logger* _loggerPtr, ShaderRegistry* _shaderRegPtr, UniformRegistry* _uniformRegPtr, ModelCache* _modelCachePtr, ViewportUI* _viewportUIPtr, MaterialCache* _materialCachePtr);
    void shutdown();
    void refreshUniforms();
    void applyAllUniformsForMaterial(unsigned int modelID);
    void setUniform(unsigned int modelID, const std::string& uniformName, UniformValue value);
    void applyInput(unsigned int modelID, const Uniform& uniform);
    void reloadUniforms(unsigned int modelID);
    bool handleEditShaderProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);
    void assignDefaultValue(Uniform& uniform);
    UniformValue getDefaultValue(UniformType type);

    void applyAllUniformsForPrimitive(unsigned int modelID, unsigned int meshID, unsigned int materialID);
    void applySceneUniforms(ShaderProgram& program);
    void applyModelUniforms(ShaderProgram& program, unsigned int modelID);
    void applyMaterialUniforms(ShaderProgram& program, unsigned int modelID, unsigned int materialID);

private:
    void applyFunction(ShaderProgram& program, const Uniform& uniform, const InspectorReference& function);
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    ShaderRegistry* shaderRegPtr = nullptr;
    UniformRegistry* uniformRegPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    ViewportUI* viewportUIPtr = nullptr;
    MaterialCache* materialCachePtr = nullptr;
    void applyUniform(unsigned int modelID, const Uniform& uniform);
    void applyUniform(ShaderProgram& program, const Uniform& uniform);
};
