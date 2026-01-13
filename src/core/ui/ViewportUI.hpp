#ifndef VIEWPORTUI_HPP
#define VIEWPORTUI_HPP

#include <glad/glad.h>
#include <memory>
#include <imgui/imgui.h>
#include "engine/Camera.hpp"

class ViewportUI {
public:
    static bool initialize();
    static void render();
    
    private:
    static bool initialized;
    static bool initPos;
    static GLuint fbo, rbo, viewportTex;
    static ImVec2 dimensions;
    static ImVec2 prevDimensions;
    static ImVec2 pos;
    static std::unique_ptr<Camera> camPtr;

    static void bind();
    static void draw();
    static void reformat();
    static float getAspect();
};

#endif