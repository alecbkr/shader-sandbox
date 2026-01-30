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
#include "object/ModelCache.hpp"
#include "core/input/InputState.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "engine/AppTimer.hpp"
#include "engine/Errorlog.hpp"

bool Application::initialized = false;

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
    
    ShaderProgram* texPtr = ShaderRegistry::getProgram("tex");
    ShaderProgram* colorPtr = ShaderRegistry::getProgram("color");

    MeshData& plane = PresetAssets::getPresetMesh(MeshPreset::PLANE);
    MeshData& cube = PresetAssets::getPresetMesh(MeshPreset::CUBE);
    MeshData& pyramid = PresetAssets::getPresetMesh(MeshPreset::PYRAMID);
    
    unsigned int gridID = ModelCache::createModel(plane.verts, plane.indices, true, false, true);
    ModelCache::setProgram(gridID, *colorPtr);
    ModelCache::scaleModel(gridID, glm::vec3(5.0f));
    // ModelCache::setTexture("grid", PresetAssets::getPresetTexture(TexturePreset::GRID), 0, "baseTex");

    unsigned int backpackID = ModelCache::createModel("../assets/models/backpack/backpack.obj");
    ModelCache::setProgram(backpackID, *texPtr);
    
    unsigned int pyramid0ID = ModelCache::createModel(pyramid.verts, pyramid.indices, true, false, true);
    ModelCache::setProgram(pyramid0ID, *colorPtr);
    
    ModelCache::translateModel(pyramid0ID, glm::vec3(3.3f, 0.0f, -1.0f));
    ModelCache::scaleModel(pyramid0ID, glm::vec3(2.0f));
    ModelCache::rotateModel(pyramid0ID, 23.2f, glm::vec3(0.0f, 1.0f, 0.0f));
    
    unsigned int pyramid1ID = ModelCache::createModel(pyramid.verts, pyramid.indices, true, false, true);
    ModelCache::setProgram(pyramid1ID, *colorPtr);
    ModelCache::translateModel(pyramid1ID, glm::vec3(-1.3f, 0.0f, -1.0f));

    unsigned int cubeID = ModelCache::createModel(cube.verts, cube.indices, true, false, true);
    ModelCache::setProgram(cubeID, *colorPtr);
    ModelCache::translateModel(cubeID, glm::vec3(4.0f, 3.0f, -5.0f));
    ModelCache::scaleModel(cubeID, glm::vec3(1.0, 0.5f, 1.0f));
    ModelCache::rotateModel(cubeID, 23.2f, glm::vec3(0.5f, 0.5f, 0.5f));
}

bool Application::initialize(AppContext& ctx) {
    if (Application::initialized) {
        ctx.logger.addLog(LogLevel::WARNING, "Application Initialization", "Application was already initialized.");
        return false;
    }
    if (!ctx.logger.initialize()) {
        std::cout << "Logger was not initialized successfully." << std::endl;
        return false;
    }
    if (!ctx.action_registry.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Action Registry was not initialized successfully.");
        return false;
    }
    if (!ctx.ctx_manager.initialize(&ctx.logger, &ctx.action_registry)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Context Manager was not initialized successfully.");
        return false;
    }
    if (!ctx.keybinds.initialize(&ctx.logger, &ctx.ctx_manager, &ctx.action_registry)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Keybinds were not initialized successfully.");
        return false;
    }
    if (!ctx.platform.initialize(&ctx.logger, &ctx.ctx_manager, &ctx.keybinds, &ctx.action_registry, ctx.width, ctx.height, ctx.app_title)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Platform layer was not initialized successfully.");
        return false;
    }
    if (!ctx.inputs.initialize(&ctx.logger, &ctx.platform)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Input State was not initialized successfully.");
        return false;
    }
    if (!ctx.timer.initialize(&ctx.logger, &ctx.platform)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "App Timer was not initialized successfully.");
        return false;
    }
    if (!ctx.events.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Event Dispatcher was not initialized successfully.");
        return false;
    }
    if (!ctx.shader_registry.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Shader Registry was not initialized successfully.");
        return false;
    }
    if (!ctx.uniform_registry.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Uniform Registry was not initialized successfully.");        
        return false;
    }
    if (!ctx.model_cache.initialize(&ctx.logger, &ctx.events, &ctx.shader_registry, &ctx.uniform_registry)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Model Cache was not initialized successfully.");        
        return false;
    }
    if (!ctx.inspector_engine.initialize(&ctx.logger, &ctx.shader_registry, &ctx.uniform_registry, &ctx.model_cache)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Inspector Engine was not initialized successfully.");        
        return false;
    }
    ctx.model_cache.setInspectorEnginePtr(&ctx.inspector_engine);
    if (!ctx.hot_reloader.initialize(&ctx.logger, &ctx.events, &ctx.shader_registry, &ctx.model_cache, &ctx.editor_engine, &ctx.inspector_engine)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Hot Reloader was not initialized successfully.");        
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