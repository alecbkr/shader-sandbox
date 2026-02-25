#include "application/Application.hpp"
#include "platform/Platform.hpp"
#include "core/ShaderRegistry.hpp"
#include <iostream>
#include "core/InspectorEngine.hpp"
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
#include "object/ModelCache.hpp"
#include "texture/TextureCache.hpp"
#include "core/input/InputState.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "engine/AppTimer.hpp"

// TEMP
#include "engine/Errorlog.hpp"

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
    // TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::WATER));
    // TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::FACE));
    // TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::METAL));
    // TextureRegistry::registerTexture(&PresetAssets::getPresetTexture(TexturePreset::GRID));
    
    ShaderProgram* texPtr = ShaderRegistry::getProgram("tex");
    ShaderProgram* colorPtr = ShaderRegistry::getProgram("color");
    ShaderProgram* gridplanePtr = ShaderRegistry::getProgram("gridplane");
    ShaderProgram* skyboxPtr = ShaderRegistry::getProgram("skybox");

    unsigned int skyboxID = ModelCache::createSkybox("../assets/textures/skybox");
    ModelCache::getModel(skyboxID)->setModelProgram(skyboxPtr->name);
    
    unsigned int gridID = ModelCache::createPreset(MeshPreset::PLANE);
    ModelCache::getModel(gridID)->setModelProgram(gridplanePtr->name);
    ModelCache::getModel(gridID)->setScale(glm::vec3(50.0f));

    // unsigned int backpackID = ModelCache::createImported("../assets/models/backpack/backpack.obj");
    // ModelCache::getModel(backpackID)->setModelProgram(texPtr->name);

    unsigned int testPlane = ModelCache::createPreset(MeshPreset::PLANE);
    ModelCache::getModel(testPlane)->addTexture("../assets/textures/grass.png", TextureType::TEX_DIFFUSE);
    ModelCache::setModelMaterialType(testPlane, 0, MaterialType::Translucent);
    ModelCache::getModel(testPlane)->setModelProgram(texPtr->name);
    ModelCache::getModel(testPlane)->rotate(90.0, glm::vec3(1.0f, 0.0f, 0.0f));
    ModelCache::getModel(testPlane)->translate(glm::vec3(0.0f, 1.0f, 0.0f));

    unsigned int testPlane2 = ModelCache::createPreset(MeshPreset::PLANE);
    ModelCache::getModel(testPlane2)->addTexture("../assets/textures/window.png", TextureType::TEX_DIFFUSE);
    ModelCache::setModelMaterialType(testPlane2, 0, MaterialType::Translucent);
    ModelCache::getModel(testPlane2)->setModelProgram(texPtr->name);
    ModelCache::getModel(testPlane2)->rotate(90.0, glm::vec3(1.0f, 0.0f, 0.0f));
    ModelCache::getModel(testPlane2)->translate(glm::vec3(-3.0f, 0.0f, 0.0f));
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
    if (!HotReloader::initialize()) {
        std::cout << "Hot Reloader was not initialized successfully." << std::endl;
    }
    if (!ModelCache::initialize()) {
        std::cout << "Object Cache was not initialized successfully." << std::endl;
        return false;
    }
    if (!TextureCache::initialize()) {
        std::cout << "Texture Cache was not initialized successfully." << std::endl;
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
    if (!EditorUI::initialize()) {
        std::cout << "Editor UI was not successfully initialized." << std::endl;
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
        ERRLOG.printClear();
        AppTimer::update();
        InputState::beginFrame();
        Platform::pollEvents();
        Platform::processInput();
        HotReloader::update();
        EventDispatcher::ProcessQueue();
        Application::renderUI();
        Platform::swapBuffers();
    }

    glfwTerminate();
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
