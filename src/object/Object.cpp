#include "Object.hpp"
#include <glm/gtc/matrix_transform.hpp>


Object::Object(std::vector<float> verts, std::vector<int> indicies, bool hasNorms, bool hasUVs) {
    renderable.mesh = std::make_unique<Mesh>(verts, indicies, hasNorms, hasUVs);
}


void Object::render() {
    renderable.mat.bindTextures();
    renderable.mesh->draw();
}


void Object::translate(glm::vec3 vector) {
    objPosition = vector;
}

void Object::scale(glm::vec3 vector) {
    objScale = vector;
}

void Object::rotate(float angle, glm::vec3 axis) {
    objOrientation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
}

glm::mat4 Object::getModelM() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, objPosition);
    model *= glm::mat4_cast(objOrientation); 
    model = glm::scale(model, objScale);
    return model;
}

GLuint Object::getProgramID() {
    return renderable.mat.getProgramID();
}

ShaderProgram* Object::getProgram() {
    return renderable.mat.program;
}

void Object::setTexture(const Texture &tex, int unit, std::string uniformName) {
    renderable.mat.setTexture(tex, unit, uniformName);
}


void Object::setProgram(ShaderProgram &program) {
    renderable.mat.setProgram(program);
}
