#ifndef VIEWPORTUI_HPP
#define VIEWPORTUI_HPP

#include <glad/glad.h>
#include "imgui/imgui.h"

class ViewportUI {
    public:
        ViewportUI();
        void bind();
        void draw();
        void reformat();
        float getAspect();
    

    private:
        bool initPos = true;
        GLuint fbo, rbo, viewportTex;
        ImVec2 dimensions = ImVec2(0, 0);
        ImVec2 prevDimensions = ImVec2(0,0);
        ImVec2 pos = ImVec2(0, 0);
};

#endif