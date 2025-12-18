#pragma once

#include "core/ui/InspectorUI.hpp"
#include "core/ui/MenuUI.hpp"
//#include "Inspector.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class UIContext {
public:
    UIContext(GLFWwindow* window);
    void preRender();
    void renderEditorWindow(float width, float height);
    void render(InspectorUI& inspectorUI);
    void render(MenuUI& menuUI);
    void postRender();
    void destroy();
};
