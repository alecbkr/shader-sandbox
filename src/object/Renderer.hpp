#pragma once

#include <glm/glm.hpp>

class Logger;
class EventDispatcher;
class ModelCache;
class MaterialCache;
class TextureCache;
class ShaderRegistry;
class UniformRegistry;
class InspectorEngine;

class Renderer {
private:
    enum QueueType {
        Opaque,
        Cutout,
        Translucent
    };

    struct Primitive {
        unsigned int modelID;
        unsigned int meshIdx;
        unsigned int materialID;
        float        depth;

        QueueType    queuetype;
    };

public:
    Renderer();
    ~Renderer() = default;
    bool initialize(
        Logger* _loggerPtr, EventDispatcher* _eventsPtr, ModelCache* _modelCachePtr, 
        MaterialCache* _materialCachePtr, TextureCache* _textureCachePtr, ShaderRegistry* _shaderRegPtr, 
        UniformRegistry* _uniformRegPtr, InspectorEngine* _inspectorEngPtr
    );

    void renderAll(glm::mat4 perspective, glm::mat4 view, glm::vec3 camPos);
    void renderModel();
    void setMeshMaterial(unsigned int modelID, unsigned int meshID, unsigned int materialID);

private:
    unsigned int nextPrimitiveID = 0;
    std::unordered_map<unsigned int, Primitive> primitiveIDMap;
    std::vector<unsigned int> opaquePrimIDs;
    std::vector<unsigned int> cutoutPrimIDs;
    std::vector<unsigned int> translucentPrimIDs;
    unsigned int skyboxPrimID;

    // void renderSkybox();
    void renderOpaquePrimitives();
    void renderCutoutPrimitives();
    void renderTranslucentPrimitives();
    void reorderTranslucentPrimitives(glm::mat4 viewMat);
    void bindTextures(unsigned int materialID);
    void bindProgram(unsigned int materialID);
    void drawMesh(unsigned int modelID, unsigned int meshID);
    bool validateNextID();
    void placeInQueue(unsigned int primitiveID, QueueType queueType);
    void removeFromQueue(unsigned int primitiveIDToDelete, QueueType queueType);

    //SYSTEM POINTERS
    Logger* loggerPtr                = nullptr;
    EventDispatcher* eventsPtr       = nullptr;
    ModelCache* modelCachePtr        = nullptr;
    MaterialCache* materialCachePtr  = nullptr;
    TextureCache* textureCachePtr    = nullptr;
    ShaderRegistry* shaderRegPtr     = nullptr;
    UniformRegistry* uniformRegPtr   = nullptr;
    InspectorEngine* inspectorEngPtr = nullptr;
};