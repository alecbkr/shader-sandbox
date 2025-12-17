#include "Material.hpp"

void Material::setTexture(Texture &tex, int unit, std::string uniformName) {
    if (tex.isValid() == false) {
        ERRLOG.logEntry(EL_WARNING, "MATERIAL", "Cannot set uninitialized texture");
        return;
    }
    textures.push_back(TextureBind{&tex, unit, uniformName});
}


void Material::bindTextures() {
    for (auto& bind : textures) {
        bind.texture->bind(bind.unit);
    }
}


void Material::setProgram(std::string programName) {
    this->programName = programName;
}