#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class Editor {
public:
    char* inputTextBuffer;
    unsigned int bufferSize;
    unsigned int width;
    unsigned int height;
    float uniformColor[3];
    unsigned int inspectorWidth;
    unsigned int inspectorHeight;
    Editor(unsigned int bufferSize, unsigned int width, unsigned int height);
    void render();
    void destroy();
};


#endif