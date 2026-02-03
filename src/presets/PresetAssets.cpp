#include "presets/PresetAssets.hpp"
#include "object/TextureType.hpp"
#include "core/logging/Logger.hpp"

PresetAssets::PresetAssets() {
    initialized = false;
    loggerPtr = nullptr;
    planeMesh.verts.clear();
    planeMesh.indices.clear();
    pyramidMesh.verts.clear();
    pyramidMesh.indices.clear();
    cubeMesh.verts.clear();
    cubeMesh.indices.clear();
    waterTex = Texture{"", TEX_DIFFUSE};
    faceTex = Texture{"", TEX_DIFFUSE};
    metalTex = Texture{"", TEX_DIFFUSE};
    gridTex = Texture{"", TEX_DIFFUSE};
}

bool PresetAssets::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Preset Assets Initialization", "Preset Assets were already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;
    PresetAssets::planeMesh.verts = {
        -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 1.0f
    };
    PresetAssets::planeMesh.indices = {
        0, 1, 2, 
        0, 2, 3
    };

    PresetAssets::pyramidMesh.verts = {
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, // bottom-left
        0.5f, 0.0f, -0.5f,  1.0f, 0.0f,  // bottom-right
        0.5f, 0.0f,  0.5f,  1.0f, 1.0f,  // top-right
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, // top-left
        0.0f, 1.0f, 0.0f,   0.5f, 0.5f   // tip
    };
    PresetAssets::pyramidMesh.indices = {
        0, 1, 2, // base 1
        0, 2, 3, // base 2
        0, 1, 4, // side 1
        1, 2, 4, // side 2
        2, 3, 4, // side 3
        3, 0, 4  // side 4
    };

    PresetAssets::cubeMesh.verts = {
        // positions       // UVs
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
        0.5f,-0.5f,-0.5f, 1.0f,0.0f,
        0.5f, 0.5f,-0.5f, 1.0f,1.0f,
        -0.5f, 0.5f,-0.5f, 0.0f,1.0f,

        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
        0.5f,-0.5f, 0.5f, 1.0f,0.0f,
        0.5f, 0.5f, 0.5f, 1.0f,1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f,1.0f
    };
    PresetAssets::cubeMesh.indices = {
        0,1,2, 0,2,3, // back
        4,5,6, 4,6,7, // front
        3,2,6, 3,6,7, // top
        0,1,5, 0,5,4, // bottom
        1,2,6, 1,6,5, // right
        0,3,7, 0,7,4  // left
    };

    initialized = true;
    return true;
}

void PresetAssets::shutdown() {
    if (!initialized) return;
    loggerPtr = nullptr;
    planeMesh.verts.clear();
    planeMesh.indices.clear();
    pyramidMesh.verts.clear();
    pyramidMesh.indices.clear();
    cubeMesh.verts.clear();
    cubeMesh.indices.clear();
    initialized = false;
}

MeshData& PresetAssets::getPresetMesh(MeshPreset preset) {
    switch (preset) {
        case MeshPreset::PLANE:
            return PresetAssets::planeMesh;
        
        case MeshPreset::CUBE:
            return PresetAssets::cubeMesh;

        case MeshPreset::PYRAMID:
            return PresetAssets::pyramidMesh;
    }
}

Texture& PresetAssets::getPresetTexture(TexturePreset preset) {
    switch (preset) {
        case TexturePreset::WATER:
            return PresetAssets::waterTex;
        
        case TexturePreset::FACE:
            return PresetAssets::faceTex;
        
        case TexturePreset::METAL:
            return PresetAssets::metalTex;
        
        case TexturePreset::GRID:
            return PresetAssets::gridTex;
    }
}