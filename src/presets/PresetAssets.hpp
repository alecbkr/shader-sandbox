#pragma once

#include <memory>
#include <vector>
#include "object/Texture.hpp"

class Logger;
class Platform;

enum class MeshPreset {
    PLANE,
    CUBE,
    PYRAMID
};

enum class TexturePreset {
    WATER,
    FACE,
    METAL,
    GRID
};

struct MeshData {
    std::vector<float> verts;
    std::vector<unsigned int> indices;
};

class PresetAssets {
public:
    PresetAssets();
    bool initialize(Logger* _loggerPtr, Platform* _platformPtr);
    void shutdown();
    MeshData& getPresetMesh(MeshPreset preset);
    Texture& getPresetTexture(TexturePreset preset);
private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    MeshData planeMesh;
    MeshData pyramidMesh;
    MeshData cubeMesh;

    Texture waterTex = Texture{"", TEX_DIFFUSE, nullptr};
    Texture faceTex = Texture{"", TEX_DIFFUSE, nullptr};
    Texture metalTex = Texture{"", TEX_DIFFUSE, nullptr};
    Texture gridTex = Texture{"", TEX_DIFFUSE, nullptr};
};
