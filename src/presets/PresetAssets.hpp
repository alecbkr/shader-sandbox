#pragma once

#include <memory>
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
    std::vector<int> indices;
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

    static std::unique_ptr<Texture> waterTex;
    static std::unique_ptr<Texture> faceTex;
    static std::unique_ptr<Texture> metalTex;
    static std::unique_ptr<Texture> gridTex;
};
