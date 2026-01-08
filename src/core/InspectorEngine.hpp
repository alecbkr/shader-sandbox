#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include "engine/ShaderProgram.hpp"
#include "core/UniformTypes.hpp"
#include "core/logging/Logger.hpp"

class InspectorEngine {
  public:
    // Organize uniforms by shader programs (outer map string is shader program name)
    static void refreshUniforms();
    static void applyInput(const std::string& objectName, const Uniform& uniform);
    static void applyAllUniformsForObject(const std::string& objectName);
    
  private:
    static void applyUniform(ShaderProgram& program, const Uniform& uniform);
    static void setUniform(const std::string& objectName, const std::string& uniformName, UniformValue value);
    static void applyUniform(const std::string& objectName, const Uniform& uniform);
    static const std::unordered_map<std::string, UniformType> typeMap;
    static std::unordered_map<std::string, Uniform> parseUniforms(const ShaderProgram& program);
    static void assignDefaultValue(Uniform& uniform);
};
