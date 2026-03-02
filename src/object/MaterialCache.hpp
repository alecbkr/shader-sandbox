#pragma once 

#include <memory>
#include <unordered_map>

class Material;
class Logger;
class MaterialCache {
    public:
        MaterialCache();
        bool initialize(Logger* _loggerPtr);
        void shutdown();
       
        void createMaterial(std::unique_ptr<Material>);
        void deleteMaterial(unsigned int materialID);
        Material* getMaterial(unsigned int materialID);
        bool contains(unsigned int materialID);
        unsigned int getNextMaterialID();
        int getSize();

    private:
        unsigned int nextMaterialID = 0;
        std::unordered_map<unsigned int, std::unique_ptr<Material>> materials;

        //SYSTEM POINTERS
        bool initialized = false;
        Logger* loggerPtr                = nullptr;
};
