#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glad/glad.h>
#include <string>
#include <vector>
#include "Texture.hpp"
#include "../engine/Errorlog.hpp"

struct TextureBind {
    Texture *texture;
    GLint unit;
    std::string uniformName; //currently does nothing
};

class Material {
    public:
        int objectID;
        std::string programName; //currently does nothing
        std::vector<TextureBind> textures;
        void setTexture(Texture &tex, int unit, std::string uniformName);
        void setProgram(std::string programNameIn);
        void bindTextures();
};

#endif