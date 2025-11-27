#ifndef VOXEL_HPP
#define VOXEL_HPP

#include "Object.hpp"

class Voxel {
    public:
        Object *obj;

        Voxel() {
            obj = nullptr;
        }

        void put(Object *newObj) {
            obj = newObj;
        }
};

#endif