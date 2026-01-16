#include "Material.hpp"
#include "core/logging/LogSink.hpp"
#include "core/logging/Logger.hpp"

void Material::setTexture(const Texture &tex, int unit, std::string uniformName) {
    if (tex.isValid() == false) {
        Logger::addLog(LogLevel::ERROR, "MATERIAL", "Cannot set uninitialized texture");
        return;
    }
    textures.push_back(TextureBind{&tex, unit, uniformName});
}


void Material::setProgram(ShaderProgram &program) {
    this->program = &program;
}


GLuint Material::getProgramID() {
    return program->ID;
}


void Material::bindTextures() {
    for (auto& bind : textures) {
        bind.texture->bind(bind.unit);
        program->setUniform_int(bind.uniformName.c_str(), bind.unit); // TODO dynamic but redundant if all objects share a program
    }
}
