#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glm/glm.hpp>

enum ObjType {
    ITEM
};

class Object {
    public:
        ObjType type;
        int size;
        glm::fvec3 color;

        Object(float r, float g, float b) {
            color.r = r;
            color.g = g;
            color.b = b;
        }
};

#endif