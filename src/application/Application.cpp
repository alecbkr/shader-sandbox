#include "application/Application.hpp"
#include "platform/Platform.hpp"
#include "core/ShaderRegistry.hpp"
#include <iostream>
#include "core/ui/InspectorUI.hpp"
#include "core/ui/EditorUI.hpp"
#include "core/ui/ConsoleUI.hpp"
#include "core/ui/ViewportUI.hpp"
#include "core/ui/MenuUI.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "core/EditorEngine.hpp"
#include "core/EventDispatcher.hpp"
#include "core/HotReloader.hpp"

bool Application::initialized = false;

void initializeUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    Platform::initializeImGui();

    ImGui_ImplOpenGL3_Init();
}

bool Application::initialize(const ApplicationInitStruct& initStruct) {
    if (Application::initialized) {
        std::cout << "Application layer already initialized." << std::endl;
        return false;
    }
    if (!Logger::initialize(initStruct.loggerSetting)) {
        std::cout << "Logger was not initialized successfully." << std::endl;
        return false;
    }
    if (!Platform::initialize({initStruct.width, initStruct.height, initStruct.title})) {
        std::cout << "Platform layer was not initialized successfully." << std::endl;
        return false;
    }
    if (!EventDispatcher::initialize()) {
        std::cout << "Event Dispatcher was not initialized successfully." << std::endl;
        return false;
    }
    if (!ShaderRegistry::initialize()) {
        std::cout << "Shader Registry was not initialized successfully." << std::endl;
        return false;
    }
    if (!EditorEngine::initialize()) {
        std::cout << "Editor Engine was not initialized successfully." << std::endl;
        return false;
    }
    
    // setup UI
    initializeUI();
    if (!ConsoleUI::initialize(Logger::getConsoleSinkPtr())) {
        std::cout << "ConsoleUI was not initialized successfully." << std::endl;
        return false;
    }
    if (!ViewportUI::initialize()) {
        std::cout << "Viewport was not initialized successfully." << std::endl;
        return false;
    }
    if (!MenuUI::initialize()) {
        std::cout << "Menu UI was not successfully initialized." << std::endl;
        return false;
    }

    Logger::addLog(LogLevel::INFO, "Application Initialization", "Application Layer Initialized.");
    Application::initialized = true;
    return true;
}

void Application::runLoop() {
    if (!Application::initialized) {
        std::cout << "Attempting to run render loop without initializing application layer." << std::endl;
        return;
    }

    while (!Application::shouldClose()) {
        Platform::pollEvents();
        Platform::processInput();
        EventDispatcher::ProcessQueue();
        Application::renderUI();
        Platform::swapBuffers();
    }
}

void Application::renderUI() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pre Render
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render UI
    InspectorUI::render();
    EditorUI::render();
    ConsoleUI::render();
    ViewportUI::render();

    // Post Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::shutdown() {
    // UI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for (Editor* editor: EditorEngine::editors) editor->destroy();
}

bool Application::shouldClose() {
    if (!initialized) return false;
    return Platform::shouldClose();
}