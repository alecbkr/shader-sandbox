#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glad/glad.h>
#include <string>
#include <vector>
#include "Texture.hpp"
#include "../engine/ShaderProgram.hpp"
#include "../core/logging/Logger.hpp"
struct TextureBind {
    Texture *texture;
    GLint unit;
    std::string uniformName; //currently does nothing
};

class Material {
    public:
        int objectID;
        ShaderProgram *program; //currently does nothing
        std::vector<TextureBind> textures;
        void bindTextures();
        void setTexture(Texture &tex, int unit, std::string uniformName);
        void setProgram(ShaderProgram &program);
        GLuint getProgramID();
};

#endif