#ifndef VIEWPORTUI_HPP
#define VIEWPORTUI_HPP

#include <glad/glad.h>
#include "imgui/imgui.h"

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
    static Camera cam;

    static void bind();
    static void draw();
    static void reformat();
    static float getAspect();
};

#endif