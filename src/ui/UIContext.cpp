#include "UIContext.hpp"

#include "core/EditorEngine.hpp"

UIContext::UIContext(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void UIContext::preRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIContext::postRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIContext::destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for (EditorUI* editor: EditorEngine::editors) editor->destroy();
}

void UIContext::renderEditorWindow(float width, float height) {
    ImGui::SetNextWindowSize(ImVec2( width, height), ImGuiCond_Once);

    if (ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) {
                    EditorEngine::spawnEditor(1024);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::BeginTabBar("EditorTabs")) {
            for (int i = 0; i < EditorEngine::editors.size(); i++) {
                std::string tabTitle = "File " + std::to_string(i + 1);
                bool openTab = true;
                if (ImGui::BeginTabItem(tabTitle.c_str(), &openTab)) {

                    EditorEngine::editors[i]->render();

                    ImGui::EndTabItem();
                }

                if (!openTab) {
                    EditorEngine::editors[i]->destroy();
                    EditorEngine::editors.erase(EditorEngine::editors.begin() + i);
                }
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

void UIContext::render(InspectorUI& inspectorUI) {
    inspectorUI.render();
}
