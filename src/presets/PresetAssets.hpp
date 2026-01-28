#pragma once

#include <vector>
#include "object/Texture.hpp"

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
    static bool initialize();
    static MeshData& getPresetMesh(MeshPreset preset);
    static Texture& getPresetTexture(TexturePreset preset);
private:
    static MeshData planeMesh;
    static MeshData pyramidMesh;
    static MeshData cubeMesh;

    static Texture waterTex;
    static Texture faceTex;
    static Texture metalTex;
    static Texture gridTex;
};