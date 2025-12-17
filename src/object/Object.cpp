#include "Object.hpp"
#include <glm/gtc/matrix_transform.hpp>


Object::Object(std::vector<float> verts, std::vector<int> indicies, bool hasNorms, bool hasUVs) {
    renderable.mesh = std::make_unique<Mesh>(verts, indicies, hasNorms, hasUVs);
}

void Object::translateObj(glm::vec3 vector) {
    position = vector;
}

void Object::scaleObj(glm::vec3 vector) {
    scale = vector;
}

void Object::rotateObj(float angle, glm::vec3 axis) {
    orientation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
}

void Object::setTexture(Texture &tex, int unit, std::string uniformName) {
    renderable.mat.setTexture(tex, unit, uniformName);
}

glm::mat4 Object::getModelM() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(orientation); //orientation not done
    model = glm::scale(model, scale);
    
    
    return model;
}

void Object::render() {
    renderable.mat.bindTextures();
    //TODO give shader model uniform
    renderable.mesh->draw();
}