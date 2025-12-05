#pragma once

#include <string>
#include <variant>
#include <glm/glm.hpp>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "engine/ShaderProgram.hpp"
#include "core/ShaderHandler.hpp"
#include "core/UniformTypes.hpp"

class InspectorEngine {
  public:
    // Organize uniforms by shader programs (outer map string is shader program name)
    static std::unordered_map<std::string, std::unordered_map<std::string, Uniform>> uniforms;
    
    InspectorEngine();
    void applyAllUniformsForProgram(const std::string& programName);
    void setUniform(const std::string& programName, const std::string& uniformName, UniformValue value);
    void applyUniform(const std::string& programName, Uniform& uniform);
    void applyUniform(ShaderProgram& program, const Uniform& uniform);
    
  private:
    static const std::unordered_map<std::string, UniformType> typeMap;
    std::unordered_map<std::string, Uniform> parseUniforms(const ShaderProgram& program);
    void assignDefaultValue(Uniform& uniform);
};