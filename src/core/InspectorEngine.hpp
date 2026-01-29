#pragma once

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include "engine/ShaderProgram.hpp"
#include "core/UniformTypes.hpp"

class InspectorEngine {
public:
    static bool initialize();
    static void refreshUniforms();
    static void applyAllUniformsForObject(unsigned int modelID);
    static void setUniform(unsigned int modelID, const std::string& uniformName, UniformValue value);
    static void applyInput(unsigned int modelID, const Uniform& uniform);
    static void reloadUniforms(const std::string& programName);
    static bool handleEditShaderProgram(const std::string& vertex_file, const std::string& fragment_file, const std::string& programName);

private:
    static void applyUniform(unsigned int modelID, const Uniform& uniform);
    static void applyUniform(ShaderProgram& program, const Uniform& uniform);
    static const std::unordered_map<std::string, UniformType> typeMap; // Kept private
    static std::unordered_map<std::string, Uniform> parseUniforms(const ShaderProgram& program);
    static void assignDefaultValue(Uniform& uniform);
};
