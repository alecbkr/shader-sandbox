// DESCRIPTION
/*
Prints miscellaneous data in a imgui widget. Feel free to add whatever
*/

#ifndef DRAWMETRICS_HPP
#define DRAWMETRICS_HPP

#include "AppTimer.hpp"
#include "InputHandler.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void drawMetrics(int appstate) {
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();

    ImGui::Begin("METRICS | Toggle with [F1]");
    ImGui::Text("FPS: %.2f", APPTIME.getFPS());
    ImGui::Text("AppState: %s | Toggle with [F2]", appstate == 0 ? "EDITOR" : "CAMERA");
    //ImGui::Text("Cursor | Pos: %.2f %.2f| Offset: %.2f %.2f", INPUT.cursor.currPosX, INPUT.cursor.currPosY, INPUT.cursor.offsetX, INPUT.cursor.offsetY);
    //ImGui::Text("Scroll | Offset: %.2f %.2f", INPUT.scroll.offsetX, INPUT.scroll.offsetY);
    //ImGui::Text("Viewport | Width: %d Height: %d", WINDOWSIZE.width, WINDOWSIZE.height);
    ImGui::End();

    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


#endif