#pragma once

#include "MaterialCategory.hpp"
#include "../engine/ShaderProgram.hpp"

class Material {
    public:
        MaterialCategory category;
        ShaderProgram *program;
};