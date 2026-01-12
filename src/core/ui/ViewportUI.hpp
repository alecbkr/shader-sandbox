#ifndef VIEWPORTUI_HPP
#define VIEWPORTUI_HPP

#include <glad/glad.h>
#include "imgui/imgui.h"

class ViewportUI {
public:
    static bool initialize();
    void bind();
    void draw();
    void reformat();
    float getAspect();


private:
    static bool initialized;
    static bool initPos;
    static GLuint fbo, rbo, viewportTex;
    static ImVec2 dimensions;
    static ImVec2 prevDimensions;
    static ImVec2 pos;
};

#endif