// UniformTypes.hpp
#pragma once

#include <optional>
#include <string>
#include <variant>
#include <glm/glm.hpp>
#include <vector>

enum class UniformType {
    NoType,
    Int,
    Float,
    Vec3,
    Vec4,
    Mat4,
    Sampler2D,
    SamplerCube
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
        case UniformType::SamplerCube: return "SamplerCube";
    }
    return "Unknown(string for this type not added yet!";
}

const std::unordered_map<std::string, UniformType> glslTypeMap = {
    {"vec3", UniformType::Vec3},
    {"vec4", UniformType::Vec4},
    {"int", UniformType::Int},
    {"float", UniformType::Float},
    {"mat4", UniformType::Mat4},
    {"sampler2D", UniformType::Sampler2D},
    {"samplerCube", UniformType::SamplerCube}
};



struct InspectorSampler2D {
    int textureUnit; 
};

struct InspectorReference {
    int modelSelection = 0; 
    int uniformSelection = 0;
    int materialSelection = 0;
    unsigned int referencedModelID;
    unsigned int referencedMaterialID;
    std::string referencedUniformName;
    UniformType returnType;
    bool useWorldData = false;
    bool useWorldVariable = false;
    bool initialized = false;
};

using UniformValue = std::variant<int, float, glm::vec3, glm::vec4, glm::mat4, InspectorSampler2D, InspectorReference>;

struct Uniform {
    std::string name;
    UniformType type;
    UniformValue value;
    unsigned int ID; // set automatically
    unsigned int materialID; // set automatically when you register the uniform
    bool isFunction = false; // right now this means reference. it won't always be this way.
    bool isReadOnly = false; // does nothing right now. 
    bool useAlternateEditor = false; // This setting is for the color picker, etc.
    bool invisible = false;
};

inline std::optional<std::vector<std::string>> getWorldData(UniformType type) {
    switch (type) {
        case UniformType::Vec3:       return std::vector<std::string>{"position", "scale"};
        case UniformType::Vec4:       return std::vector<std::string>{"orientation"};
        //case UniformType::Float:      return std::vector<std::string>{"getTime"};
        default: return std::nullopt;
    }
    return std::nullopt;
}


