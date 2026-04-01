#pragma once 

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "object/Material.hpp"


class Logger;
class EventDispatcher;
class TextureCache;


class MaterialCache {
public:
    MaterialCache();
    bool initialize(Logger* _loggerPtr, EventDispatcher* _eventsPtr, TextureCache* _textureCachePtr, bool previouslySaved);
    void shutdown();
    
    unsigned int createBlankMaterial();
    unsigned int createMaterial(MaterialType type, MaterialProperties properties, std::vector<unsigned int> TextureIDs);
    void deleteMaterial(unsigned int materialID);
    

    void addTextureToMaterial(unsigned int materialID, std::string texture_path, bool isCubemap);
    void removeTextureFromMaterial(unsigned int materialID, unsigned int textureID);
    bool changeMaterialName(unsigned int materialID, std::string name);
    void changeMaterialType(unsigned int materialID, MaterialType type);
    bool loadMaterialFromSave(unsigned int ID, MaterialType type, MaterialProperties properties, std::vector<std::string> texture_paths);

    Material* getMaterial(unsigned int materialID);
    std::vector<unsigned int> getAllMaterialIDs();
    std::vector<Material*> getAllMaterials();
    std::vector<std::string> getAllTexturePathsForMaterial(unsigned int materialID);

    bool contains(unsigned int materialID);
    unsigned int getNextMaterialID();
    int getSize();

private:
    unsigned int nextMaterialID = 0;
    std::unordered_map<unsigned int, std::unique_ptr<Material>> materialIDMap;
    std::unordered_set<std::string> usedMaterialNames;
    bool validateNextID();

    //SYSTEM POINTERS
    bool initialized = false;
    Logger* loggerPtr             = nullptr;
    EventDispatcher* eventsPtr    = nullptr;
    TextureCache* textureCachePtr = nullptr;
};
