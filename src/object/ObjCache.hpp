#ifndef OBJCACHE_HPP
#define OBJCACHE_HPP

#include "Object.hpp"
#include <vector>


class ObjCache {
    public:
        ObjCache() = delete;
        // static void OC_addObj(Object &obj);
        static void createObj(const std::string& name, std::vector<float>, std::vector<int>, 
                                 bool hasNorms, bool hasUVs, ShaderProgram& program);
        // static void OC_deleteObj();
        
        static void setTexture(const std::string name, Texture& tex, int unit, std::string uniformName);
        static void setProgram(const std::string name, ShaderProgram& program);
        static void setName(const std::string name, const std::string newName);
        static void translateObj(const std::string name, glm::vec3 pos);
        static void scaleObj(const std::string name, glm::vec3 scale);
        static void rotateObj(const std::string name, float angle, glm::vec3 axis);
        
        static void renderObj(const std::string name, glm::mat4 projection, glm::mat4 view);
        static void renderAll(glm::mat4 projection, glm::mat4 view);
        static void printOrder();
        static std::unordered_map<std::string, Object*> objMap;

    private:
        static Object* getObject(const std::string name);
        static void reorderByProgram();
        static std::vector<std::unique_ptr<Object>> objCache;
        
};

#endif
