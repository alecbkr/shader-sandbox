#ifndef UICONTEXT_HPP
#define UICONTEXT_HPP

#include "Editor.hpp"
#include "core/ui/InspectorUI.hpp"
//#include "Inspector.hpp"
#include "core/ui/ConsoleUI.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class UIContext {
public:
    UIContext(GLFWwindow* window);
    void preRender();
    void render(Editor* editor);
    void render(InspectorUI& inspectorUI);
    void render(ConsoleUI& consoleUI); 
    void postRender();
    void destroy(Editor* editor);
};


#endif
