// ENGINE
#include "core/InspectorEngine.hpp"
#include "engine/Window.hpp"
#include "engine/Errorlog.hpp"
#include "engine/InputHandler.hpp"
#include "engine/AppTimer.hpp"
#include "engine/Camera.hpp"
#include "engine/DrawMetrics.hpp"
#include "core/EventDispatcher.hpp"
#include "object/Object.hpp"
#include "core/TextureRegistry.hpp"
#include "engine/ShaderProgram.hpp"

#include "ui/UIContext.hpp"
#include "core/ui/InspectorUI.hpp"
#include "core/ShaderHandler.hpp"
#include "core/EditorEngine.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "object/ObjCache.hpp"


void processInput(GLFWwindow *window);
void cameraControls(GLFWwindow *window, Camera &camera);
void editorControls(GLFWwindow *window);


enum AppState {
    AS_EDITOR,
    AS_CAMERA
};


// GLOBAL VARIABLES
Camera cam;
AppState appstate = AS_EDITOR;
bool showMetrics = true;
std::vector<EditorUI*> EditorEngine::editors;

int main() {
    Window win("Sandbox", 1000, 800);
    ShaderHandler shaderHandler;

    EditorEngine::spawnEditor(1024);

    UIContext ui(win.window);
    InspectorUI inspectorUI;

    MenuUI menuUI = MenuUI();



    // GRIDPLANE
    std::vector<float> gridPlane_verts {
        -1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f, -1.0f, 0.0f, 1.0f
    };

    std::vector<int> gridPlane_indices {
        0, 1, 2, 
        0, 2, 3
    };

    // PYRAMID
    std::vector<float> pyramidVerts = {
        // Base
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, // 0: bottom-left
        0.5f, 0.0f, -0.5f,  1.0f, 0.0f, // 1: bottom-right
        0.5f, 0.0f,  0.5f,  1.0f, 1.0f, // 2: top-right
        -0.5f, 0.0f,  0.5f,  0.0f, 1.0f, // 3: top-left

        // Apex
        0.0f, 1.0f, 0.0f,   0.5f, 0.5f  // 4: tip
    };

    std::vector<int> pyramidIndices = {
        0, 1, 2,  0, 2, 3, // base
        0, 1, 4,            // side 1
        1, 2, 4,            // side 2
        2, 3, 4,            // side 3
        3, 0, 4             // side 4
    };

    // CUBE
    std::vector<float> cubeVerts = {
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

    // Indices for cube (two triangles per face)
    std::vector<int> cubeIndices = {
        0,1,2, 0,2,3, // back
        4,5,6, 4,6,7, // front
        3,2,6, 3,6,7, // top
        0,1,5, 0,5,4, // bottom
        1,2,6, 1,6,5, // right
        0,3,7, 0,7,4  // left
    };

    
    // TEXTURES
    Texture waterTex("../assets/textures/water.png");
    Texture faceTex("../assets/textures/bigface.jpg");
    Texture edgeTex("../assets/textures/rim.png");
    Texture gridTex("../assets/textures/grid.png");
    TEXTURE_REGISTRY.registerTexture(&waterTex);
    TEXTURE_REGISTRY.registerTexture(&faceTex);
    TEXTURE_REGISTRY.registerTexture(&edgeTex);
    TEXTURE_REGISTRY.registerTexture(&gridTex);

    // PROGRAMS
    ShaderHandler::registerProgram("../shaders/3d.vert", "../shaders/texture.frag", "program");
    ShaderHandler::registerProgram("../shaders/default.vert", "../shaders/default.frag", "untex");


    ShaderProgram* programPtr = ShaderHandler::getProgram("program");
    ShaderProgram* untexPtr = ShaderHandler::getProgram("untex");
    if (programPtr == nullptr || untexPtr == nullptr) {
        ERRLOG.logEntry(EL_CRITICAL, "main", "pointer not registered properly?");
    }
    ShaderProgram& program = *programPtr;
    ShaderProgram& untex = *untexPtr;

    
    // OBJECTS
    ObjCache::createObj("grid", gridPlane_verts, gridPlane_indices, false, true, program);
    ObjCache::setTexture("grid", gridTex, 0, "baseTex");
    ObjCache::scaleObj("grid", glm::vec3(5.0f));

    ObjCache::createObj("pyramid0", pyramidVerts, pyramidIndices, false, true, untex);
    ObjCache::translateObj("pyramid0", glm::vec3(3.3f, 0.0f, -1.0f));
    ObjCache::scaleObj("pyramid0", glm::vec3(2.0f));
    ObjCache::rotateObj("pyramid0", 23.2f, glm::vec3(0.0f, 1.0f, 0.0f));

    ObjCache::createObj("cube", cubeVerts, cubeIndices, false, true, program);
    ObjCache::setTexture("cube", faceTex, 0, "baseTex");
    ObjCache::translateObj("cube", glm::vec3(4.0f, 3.0f, -5.0f));
    ObjCache::scaleObj("cube", glm::vec3(1.0, 0.5f, 1.0f));
    ObjCache::rotateObj("cube", 45.0f, glm::vec3(0.5f, 0.5f, 0.5f));

    ObjCache::createObj("pyramid1", pyramidVerts, pyramidIndices, false, true, untex);
    ObjCache::translateObj("pyramid1", glm::vec3(-1.3f, 0.0f, -1.0f));


    ERRLOG.printClear();
    glClearColor(0.4f, 0.1f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    InspectorEngine::refreshUniforms();

    // RUN LOOP
    while (!win.shouldClose()) {
        glfwPollEvents();
        INPUT.updateCursor();
        processInput(win.window);

        EventDispatcher::ProcessQueue();

        ui.preRender();
        ui.render(menuUI);
        ui.renderEditorWindow(500, 500);
        ui.render(inspectorUI);


        // ---DRAWING---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)win.width / (float)win.height, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        ObjCache::renderAll(perspective, view);

        
        if (showMetrics) drawMetrics(appstate);
        ui.postRender();


        // ---END-OF-FRAME---
        INPUT.resetStates();
        APPTIME.update();
        ERRLOG.printClear();
        win.swapBuffers();
    }

    ui.destroy();
}


// =====CONTROLS=====

void processInput(GLFWwindow *window) {
    switch (appstate) {
        case AS_EDITOR: editorControls(window);      break;
        case AS_CAMERA: cameraControls(window, cam); break;
        default:        editorControls(window);      break;
    }
}


void cameraControls(GLFWwindow *window, Camera &camera) {
    // [ESC] - exit app
    if (KEYBOARD[GLFW_KEY_ESCAPE].isPressed) {
        glfwSetWindowShouldClose(window, true);
    }
    // [F1] - show metrics
    if (KEYBOARD[GLFW_KEY_F1].isPressed) {
        showMetrics = !showMetrics;
    }
    // [F2] - switch to editor controls
    if (KEYBOARD[GLFW_KEY_F2].isPressed) {
        appstate = AS_EDITOR;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    //camera movements
    if (KEYBOARD[GLFW_KEY_W].isDown) {
        camera.ProcessKeyboard(CAM_FORWARD, APPTIME.getDt());
    }
    if (KEYBOARD[GLFW_KEY_A].isDown) {
        camera.ProcessKeyboard(CAM_LEFT, APPTIME.getDt());
    }
    if (KEYBOARD[GLFW_KEY_S].isDown) {
        camera.ProcessKeyboard(CAM_BACKWARD, APPTIME.getDt());
    }
    if (KEYBOARD[GLFW_KEY_D].isDown) {
        camera.ProcessKeyboard(CAM_RIGHT, APPTIME.getDt());
    }
    cam.ProcessMouseMovement(CURSOR.offsetX, CURSOR.offsetY, true);
}


void editorControls(GLFWwindow *window) {
    // [ESC] - exit app
    if (KEYBOARD[GLFW_KEY_ESCAPE].isPressed) {
        glfwSetWindowShouldClose(window, true);
    }
    // [F1] - show metrics
    if (KEYBOARD[GLFW_KEY_F1].isPressed) {
        showMetrics = !showMetrics;
    }
    // [F2] - switch to camera controls
    if (KEYBOARD[GLFW_KEY_F2].isPressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        CURSOR.firstInput = true;
        appstate = AS_CAMERA;
    }
}
