// UniformTypes.hpp
#pragma once

#include <string>
#include <variant>
#include <glm/glm.hpp>

enum class UniformType {
    NoType,
    Int,
    Float,
    Vec3,
    Vec4,
    Mat4,
    Sampler2D,
    UniformRef
};

inline std::string to_string(UniformType type) {
    switch (type) {
        case UniformType::NoType:     return "NoType";
        case UniformType::Int:        return "Int";
        case UniformType::Float:      return "Float";
        case UniformType::Vec3:       return "Vec3";
        case UniformType::Vec4:       return "Vec4";
        case UniformType::Mat4:       return "Mat4";
        case UniformType::Sampler2D:  return "Sampler2D";
        case UniformType::UniformRef: return "UniformRef";
    }
    return "Unknown(string for this type not added yet!";
}

struct InspectorSampler2D {
    int textureUnit; 
};

using UniformValue = std::variant<int, float, glm::vec3, glm::vec4, glm::mat4, InspectorSampler2D>;

struct UniformRef {
    std::string shaderName;
    std::string uniformName;
    std::string uniformType;
};

struct Uniform {
    std::string name;
    UniformType type;
    UniformValue value;
    UniformRef ref;
    bool isReadOnly = false;
    // This setting is for the color picker, etc.
    bool useAlternateEditor = false;
};

