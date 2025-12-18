#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine/Window.hpp"
#include "engine/ShaderProgram.hpp"
#include "engine/Errorlog.hpp"
#include "ui/UIContext.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ui/InspectorUI.hpp"
#include "core/ShaderHandler.hpp"
#include "core/HotReloader.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


int main() {
    Window window("Process", 400, 400);

    ShaderHandler shaderHandler;
    InspectorUI inspectorUI;
    InspectorEngine inspectorEngine;
    HotReloader reloader(&shaderHandler, &inspectorEngine);
    UIContext ui(window.window);

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

    bool R_key_held = false;
    const std::string FRAG_PATH = "../shaders/default.frag";

    ERRLOG.printClear();
    glClearColor(0.4f, 0.1f, 0.0f, 1.0f);

    // RUN LOOP
    while (!window.shouldClose()) {
        // if (glfwGetKey(window.window, GLFW_KEY_R)) inspector.refreshShaders();
        glfwPollEvents();

        ui.preRender();
        ui.render(editor);
        ui.render(inspectorUI);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool R_key_pressed = glfwGetKey(window.window, GLFW_KEY_R) == GLFW_PRESS;

        if (R_key_pressed && !R_key_held){
            editor->saveToFile(FRAG_PATH);
            reloader.compile(FRAG_PATH);
        }

        R_key_held = R_key_pressed;
        auto& programs = ShaderHandler::getPrograms();
        for (auto& [programName, programPointer] : programs) {
            programPointer->use();
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        ui.postRender();

        // End of loop events
        window.swapBuffers();
        ERRLOG.printClear();
    }

    ui.destroy(editor);
}
