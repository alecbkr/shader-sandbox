#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine/Window.hpp"
#include "engine/ShaderProgram.hpp"
#include "engine/Errorlog.hpp"
#include "ui/UIContext.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


int main() {
    
    Window window("Process", 400, 400);

    ShaderProgram shader("../shaders/default.vert", "../shaders/default.frag");

    UIContext* ui = new UIContext(window.window);


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

    // RUN LOOP
    float zoomOut = 10.0f;
    glClearColor(0.4f, 0.1f, 0.0f, 1.0f);
    while (!window.shouldClose()) {
        glfwPollEvents();

        ui->preRender();
        ui->render(editor);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setUniform_float("zoom", zoomOut);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        ui->postRender();

        // End of loop events
        window.swapBuffers();
        ERRLOG.printClear();
    }

    ui->destroy(editor);
}
