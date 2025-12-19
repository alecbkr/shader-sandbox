#include "ObjCache.hpp"
#include <algorithm>
#include <iostream>

std::vector<std::unique_ptr<Object>> ObjCache::objCache;
std::unordered_map<std::string, Object*> ObjCache::objMap;


// void ObjCache::OC_addObj(Object &obj) {

//     objCache.push_back(std::make_unique<Object>(std::move(obj)));
//     // if (objCache.size() > 1) {
//     //     std::sort(objCache.begin(), objCache.end(), 
//     //         [](const auto& a, const auto& b) {
//     //             return a->renderable.mat.program->ID < b->renderable.mat.program->ID;
//     //     });
//     // }
// }

void ObjCache::createObj(const std::string& name, std::vector<float> verts, std::vector<int> indices, 
                            bool hasNorms, bool hasUVs, ShaderProgram &program) {

    if (getObject(name) != nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object creation failed. Name already in use:", name.c_str());
        return;
    }
    
    auto obj = std::make_unique<Object>(Object(verts, indices, hasNorms, hasUVs));
    objMap[name] = &(Object&)(*obj);
    obj.get()->setProgram(program);

    objCache.push_back(std::move(obj));
    reorderByProgram();
}


void ObjCache::translateObj(const std::string name, glm::vec3 pos) {
    Object* obj = getObject(name);
    if (obj == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object not found:", name.c_str());
        return;
    }
    obj->translate(pos);
}


void ObjCache::scaleObj(const std::string name, glm::vec3 scale) {
    Object* obj = getObject(name);
    if (obj == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object not found:", name.c_str());
        return;
    }
    obj->scale(scale);
}


void ObjCache::rotateObj(const std::string name, float angle, glm::vec3 axis) {
    Object* obj = getObject(name);
    if (obj == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object not found:", name.c_str());
        return;
    }
    obj->rotate(angle, axis);
}


void ObjCache::setTexture(const std::string name, Texture& tex, int unit, std::string uniformName) {
    Object* obj = getObject(name);
    if (obj == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object not found:", name.c_str());
        return;
    }

    obj->setTexture(tex, unit, uniformName);
}


void ObjCache::setProgram(const std::string name, ShaderProgram &program) {
    Object* obj = getObject(name);
    if (obj == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object not found:", name.c_str());
        return;
    }

    obj->setProgram(program);
    reorderByProgram();
}


void ObjCache::renderObj(std::string name, glm::mat4 perspective, glm::mat4 view) {
    Object* obj = getObject(name);
    if (obj == nullptr) {
        ERRLOG.logEntry(EL_WARNING, "OBJECT CACHE", "Object not found:", name.c_str());
        return;
    }

    ShaderProgram* currProgram = obj->getProgram();
    currProgram->use();
    currProgram->setUniform_mat4float("projection", perspective);
    currProgram->setUniform_mat4float("view", view);
    currProgram->setUniform_mat4float("model", obj->getModelM());
    obj->render();
}


void ObjCache::renderAll(glm::mat4 perspective, glm::mat4 view) {
    ShaderProgram *currProgram = objCache[0].get()->getProgram();
    currProgram->use();
    currProgram->setUniform_mat4float("projection", perspective);
    currProgram->setUniform_mat4float("view", view);

    for (auto& currObj : objCache) {
        if (currProgram->ID != currObj.get()->getProgramID()) {
            currProgram = currObj.get()->getProgram();

            currProgram->use();
            currProgram->setUniform_mat4float("projection", perspective);
            currProgram->setUniform_mat4float("view", view);
        }

        currProgram->setUniform_mat4float("model", currObj.get()->getModelM());
        currObj.get()->render();
    }
}


void ObjCache::printOrder() {
    for (auto& obj : objCache) {
        std::cout << obj->renderable.mat.program->ID << std::endl;
    }
}


Object* ObjCache::getObject(const std::string name) {
    return objMap[name];
}


void ObjCache::reorderByProgram() {
    std::sort(objCache.begin(), objCache.end(),
              [](const std::unique_ptr<Object>& a, const std::unique_ptr<Object>& b) {
                  return a.get()->getProgramID() < b.get()->getProgramID(); // dereference the unique_ptr
              });
}