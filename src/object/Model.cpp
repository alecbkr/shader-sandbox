#include "Model.hpp"
#include "ModelImporter.hpp"
#include "../engine/ErrorLog.hpp"


Model::Model(const unsigned int ID) : ID(ID) {

}


// -----FUNCTIONALITY
void Model::renderModel() {
    if (properties.hasMeshes == false) {
        ERRLOG.logEntry(EL_WARNING, "MODEL", "Render failure, no meshes present");
        return;
    }

    if (properties.hasProgram == false) {
        ERRLOG.logEntry(EL_WARNING, "MODEL", "Render failure, no shader program present");
        return;
    }

    program->use();
    for (MeshA& mesh : all_meshes) {
        mesh.bind();
        bindTextures(mesh);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    }
}


void Model::unloadModel() {
    if (properties.hasMeshes) {
        for (MeshA& mesh : all_meshes) {
            mesh.unloadFromGPU();
        }
    }
}


// -----TRANSLATIONS
void Model::translate(glm::vec3 position) {
    this->position = position;
    this->modelM = calcModelM();
}


void Model::rescale(glm::vec3 scale) {
    this->scale = scale;
    this->modelM = calcModelM();
}


void Model::rotate(float angle, glm::vec3 axis) {
    this->orientation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    this->modelM = calcModelM();
}



// -----SETTERS
void Model::setProgram(ShaderProgram &program) {
    this->program = &program;
    properties.hasProgram = true;
}


void Model::setMesh(std::vector<float> vertices, std::vector<unsigned int> indices, bool hasPos, bool hasNorm, bool hasUV) {

}


void Model::addTexture(std::string pathname) {
    
}


glm::mat4 Model::calcModelM() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(orientation); 
    model = glm::scale(model, scale);
    return model;
}


ShaderProgram* Model::getProgram() {
    return program;
}


const int Model::getID() {
    return ID;
}


void Model::bindTextures(MeshA& mesh) {
    if (mesh.meshflags.hasTextures == false) return;

    for (auto& texture : mesh.textures) {
        //TODO needs to go through uniform handler and grab the uniform Sampler2Ds to properly bind textures
        //placeholder starts
        switch (texture->getType()) {
            case TEX_DIFFUSE: 
                texture->bind(0);
                // program->setUniform_int("base", 0);
                break;

            case TEX_SPECULAR:
                break;
        }
        //placeholder ends
    }
        
}