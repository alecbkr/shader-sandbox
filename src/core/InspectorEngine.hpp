#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include "engine/ShaderProgram.hpp"
#include "core/UniformTypes.hpp"
#include "UniformRegistry.hpp"

class InspectorEngine {
  public:
    // Organize uniforms by shader programs (outer map string is shader program name)
    InspectorEngine(UniformRegistry& registry);
    void applyAllUniformsForProgram(const std::string& programName);
    void setUniform(const std::string& programName, const std::string& uniformName, UniformValue value);
    void applyUniform(const std::string& programName, const Uniform& uniform);
    void applyUniform(ShaderProgram& program, const Uniform& uniform);
    
  private:
    UniformRegistry& uniformRegistry;

    static const std::unordered_map<std::string, UniformType> typeMap;
    std::unordered_map<std::string, Uniform> parseUniforms(const ShaderProgram& program);
    void assignDefaultValue(Uniform& uniform);
};
