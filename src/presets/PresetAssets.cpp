#include "presets/PresetAssets.hpp"
#include "texture/TextureType.hpp"
#include "core/logging/Logger.hpp"
#include "platform/Platform.hpp"

PresetAssets::PresetAssets() {
    initialized = false;
    loggerPtr = nullptr;
    planeMesh.verts.clear();
    planeMesh.indices.clear();
    pyramidMesh.verts.clear();
    pyramidMesh.indices.clear();
    cubeMesh.verts.clear();
    cubeMesh.indices.clear();
    // waterTex = Texture{"", TEX_DIFFUSE, nullptr};
    // faceTex = Texture{"", TEX_DIFFUSE, nullptr};
    // metalTex = Texture{"", TEX_DIFFUSE, nullptr};
    // gridTex = Texture{"", TEX_DIFFUSE, nullptr};
}

bool PresetAssets::initialize(Logger* _loggerPtr, Platform* _platformPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Preset Assets Initialization", "Preset Assets were already initialized.");
        return false;
    }

    loggerPtr = _loggerPtr;

    // waterTex = Texture{(_platformPtr->getExeDir() / ".." / "assets/textures/water.png").string().c_str(), TEX_DIFFUSE, loggerPtr};
    // faceTex = Texture{(_platformPtr->getExeDir() / ".." / "assets/textures/bigface.jpg").string().c_str(), TEX_DIFFUSE, loggerPtr};
    // metalTex = Texture{(_platformPtr->getExeDir() / ".." / "assets/textures/rim.png").string().c_str(), TEX_DIFFUSE, loggerPtr};
    // gridTex = Texture{(_platformPtr->getExeDir() / ".." / "assets/textures/grid.png").string().c_str(), TEX_DIFFUSE, loggerPtr};

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
    //front:
        -0.5f,-0.5f, 0.5f, 0.0f,0.0f, //bottom-left
        0.5f,-0.5f, 0.5f, 1.0f,0.0f,  //bottom-right
        0.5f, 0.5f, 0.5f, 1.0f,1.0f,  //top-right
        -0.5f, 0.5f, 0.5f, 0.0f,1.0f, //top-left
    
    //back:
        0.5f,-0.5f,-0.5f, 0.0f,0.0f,
        -0.5f,-0.5f,-0.5f, 1.0f,0.0f,
        -0.5f, 0.5f,-0.5f, 1.0f,1.0f,
        0.5f, 0.5f,-0.5f, 0.0f,1.0f,

    //left: 
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
        -0.5f,-0.5f, 0.5f, 1.0f,0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f,1.0f,
        -0.5f, 0.5f,-0.5f, 0.0f,1.0f,
    
    //right:
        0.5f,-0.5f, 0.5f, 0.0f,0.0f,
        0.5f,-0.5f,-0.5f, 1.0f,0.0f,
        0.5f, 0.5f,-0.5f, 1.0f,1.0f,
        0.5f, 0.5f, 0.5f, 0.0f,1.0f,

    //top:
        -0.5f, 0.5f, 0.5f, 0.0f,0.0f,
        0.5f, 0.5f, 0.5f, 1.0f,0.0f,
        0.5f, 0.5f,-0.5f, 1.0f,1.0f,
        -0.5f, 0.5f,-0.5f, 0.0f,1.0f,

    //bottom:
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
        0.5f,-0.5f,-0.5f, 1.0f,0.0f,
        0.5f,-0.5f, 0.5f, 1.0f,1.0f,
        -0.5f,-0.5f, 0.5f, 0.0f,1.0f
    };
    PresetAssets::cubeMesh.indices = {
        0,1,2,    2,3,0,    // front
        4,5,6,    6,7,4,    // back
        8,9,10,   10,11,8,  // left
        12,13,14, 14,15,12, // right
        16,17,18, 18,19,16, // top
        20,21,22, 22,23,20  // bottom
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
        
        default: return PresetAssets::cubeMesh;
    }
}

// Texture& PresetAssets::getPresetTexture(TexturePreset preset) {
//     switch (preset) {
//         case TexturePreset::WATER:
//             return PresetAssets::waterTex;
        
//         case TexturePreset::FACE:
//             return PresetAssets::faceTex;
        
//         case TexturePreset::METAL:
//             return PresetAssets::metalTex;
        
//         case TexturePreset::GRID:
//             return PresetAssets::gridTex;
//     }
// }
