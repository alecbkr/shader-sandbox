// ENGINE
#include "engine/Window.hpp"
#include "engine/ShaderProgram.hpp"
#include "engine/Errorlog.hpp"
#include "engine/InputHandler.hpp"
#include "engine/AppTimer.hpp"
#include "engine/Camera.hpp"
#include "engine/DrawMetrics.hpp"

#include "ui/UIContext.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ui/InspectorUI.hpp"
#include "core/ui/ConsoleUI.hpp"
#include "core/ShaderHandler.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


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


int main() {
    Window win("Sandbox", 1000, 800);
    ShaderHandler shaderHandler;
    InspectorUI inspectorUI;
    ConsoleUI consoleUI; 
    UIContext ui(win.window);

    GLfloat voxel_verts[] = {
        -1.0,  1.0, 0.0, // TOP-LEFT
         1.0,  1.0, 0.0, // TOP-RIGHT
         1.0, -1.0, 0.0, // BOTTOM-RIGHT
        -1.0, -1.0, 0.0  // BOTTOM-LEFT
    };

    GLint voxel_indices[] = {
        0, 1, 2,
        0, 2, 3
    };


    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(voxel_verts), voxel_verts, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(voxel_indices), voxel_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);


    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Construct ImGui elements
    Editor* editor = new Editor(1024, 200, 200);

    ERRLOG.printClear();
    glClearColor(0.4f, 0.1f, 0.0f, 1.0f);

    // RUN LOOP
    while (!win.shouldClose()) {
        glfwPollEvents();
        processInput(win.window);

        ui.preRender();
        ui.render(editor);
        ui.render(inspectorUI);
        ui.render(consoleUI); 

        // ---DRAWING---
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)win.width / (float)win.height, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();

        auto& programs = ShaderHandler::getPrograms();
        for (auto& [programName, program] : programs) {
            program.use();
            
            program.setUniform_mat4float("projection", perspective);
            program.setUniform_mat4float("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
            model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            program.setUniform_mat4float("model", model);

            program.setUniform_vec3float("inColor", 1.0f, 0.0f, 0.4f);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        if (showMetrics) drawMetrics(appstate);

        ui.postRender();


        // ---END-OF-FRAME---
        INPUT.resetStates();
        APPTIME.update();
        ERRLOG.printClear();
        win.swapBuffers();
    }

    ui.destroy(editor);
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
    // [TAB] - switch to editor controls
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
    // [TAB] - switch to camera controls
    if (KEYBOARD[GLFW_KEY_F2].isPressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        CURSOR.firstInput = true;
        appstate = AS_CAMERA;
    }
}