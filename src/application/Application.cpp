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
#include "presets/PresetAssets.hpp"
#include "core/TextureRegistry.hpp"
#include "core/FileRegistry.hpp"
#include "object/ObjCache.hpp"
#include "core/input/InputState.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "engine/AppTimer.hpp"

bool Application::initialized = false;
AppStateControls Application::appControls = AppStateControls::NO_STATE;

void initializeUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    Platform::initializeImGui();

    ImGui_ImplOpenGL3_Init();
}

void loadPresetAssets() {
    TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::WATER));
    TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::FACE));
    TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::METAL));
    TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::GRID));

    ShaderProgram* programPtr = ShaderRegistry::getProgram("program");
    ShaderProgram* untexPtr = ShaderRegistry::getProgram("untex");

    MeshData& plane = PresetAssets::getPresetMesh(MeshPreset::PLANE);
    MeshData& cube = PresetAssets::getPresetMesh(MeshPreset::CUBE);
    MeshData& pyramid = PresetAssets::getPresetMesh(MeshPreset::PYRAMID);

    ObjCache::createObj("grid", plane.verts, plane.indices, false, true, *programPtr);
    ObjCache::setTexture("grid", PresetAssets::getPresetTexture(TexturePreset::GRID), 0, "baseTex");
    ObjCache::scaleObj("grid", glm::vec3(5.0f));

    ObjCache::createObj("pyramid0", pyramid.verts, pyramid.indices, false, true, *untexPtr);
    ObjCache::translateObj("pyramid0", glm::vec3(3.3f, 0.0f, -1.0f));
    ObjCache::scaleObj("pyramid0", glm::vec3(2.0f));
    ObjCache::rotateObj("pyramid0", 23.2f, glm::vec3(0.0f, 1.0f, 0.0f));

    ObjCache::createObj("cube", cube.verts, cube.indices, false, true, *programPtr);
    ObjCache::setTexture("cube", PresetAssets::getPresetTexture(TexturePreset::FACE), 0, "baseTex");
    ObjCache::translateObj("cube", glm::vec3(4.0f, 3.0f, -5.0f));
    ObjCache::scaleObj("cube", glm::vec3(1.0, 0.5f, 1.0f));
    ObjCache::rotateObj("cube", 45.0f, glm::vec3(0.5f, 0.5f, 0.5f));

    ObjCache::createObj("pyramid1", pyramid.verts, pyramid.indices, false, true, *untexPtr);
    ObjCache::translateObj("pyramid1", glm::vec3(-1.3f, 0.0f, -1.0f));
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
    if (!AppTimer::initialize()) {
        std::cout << "App Timer was not initialized successfully." << std::endl;
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
    if (!FileRegistry::initialize()) {
        std::cout << "File Registry was not initialized successfully." << std::endl;
        return false;
    }
    if (!EditorEngine::initialize()) {
        std::cout << "Editor Engine was not initialized successfully." << std::endl;
        return false;
    }
    if (!PresetAssets::initialize()) {
        std::cout << "Preset Assets were not initialized successfully." << std::endl;
        return false;
    }
    loadPresetAssets();
    if (!InspectorEngine::initialize()) {
        std::cout << "Inspector Engine was not initialized successfully." << std::endl;
        return false;
    }
    if (!InputState::initialize()) {
        std::cout << "Input State was not initialized successfully." << std::endl;
        return false;
    }
    if (!ActionRegistry::initialize()) {
        std::cout << "Action Registry was not initialized successfully." << std::endl;
        return false;
    }
    if (!ContextManager::initialize()) {
        std::cout << "Context Manager was not initialized successfully." << std::endl;
        return false;
    }
    if (!Keybinds::initialize()) {
        std::cout << "Keybinds were not initialized successfully." << std::endl;
        return false;
    }
    // setup UI
    initializeUI();
    if (!ConsoleUI::initialize(Logger::getConsoleSinkPtr())) {
        std::cout << "ConsoleUI was not initialized successfully." << std::endl;
        return false;
    }
    if (!ViewportUI::initialize()) {
        std::cout << "Viewport UI was not initialized successfully." << std::endl;
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
        AppTimer::update();
        InputState::beginFrame();
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
    MenuUI::render();

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

void Application::setAppStateControls(AppStateControls state) {
    if (!initialized) return;
    Application::appControls = state;
}

AppStateControls Application::checkAppStateControls() {
    if (!initialized) return AppStateControls::NO_STATE;
    return appControls;
}