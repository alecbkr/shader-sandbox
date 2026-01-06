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
    UniformRef
};

using UniformValue = std::variant<int, float, glm::vec3, glm::vec4>;

struct UniformRef {
    std::string shaderName;
    std::string uniformName;
    std::string uniformType;
};

struct Uniform {
    std::string name;
    UniformType type;
    UniformValue value;
    bool wasUniformRef = false;
    UniformRef ref;
};

