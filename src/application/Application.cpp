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
#include "core/input/InputState.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "engine/AppTimer.hpp"
#include "engine/Errorlog.hpp"
#include "persistence/ProjectLoader.hpp"

bool Application::initialized = false;
std::size_t Application::fontIdx = 2;
std::array<ImFont*, 6> Application::fonts{};

void Application::increaseFont() { if (fontIdx < 5) fontIdx++; }
void Application::decreaseFont() { if (fontIdx > 0) fontIdx--; }

void subscribeMenuButtons(AppContext& ctx) {
    ctx.events.Subscribe(EventType::SaveProject, [&ctx](const EventPayload&) -> bool {
        ProjectLoader::save(ctx.project);
        return false;
    });
    ctx.events.Subscribe(EventType::Quit, [&ctx](const EventPayload&) -> bool {
        ctx.shouldClose = true;
        return true;
    });
}

bool Application::addDefaultActionBinds(ActionRegistry* actionRegPtr, ViewportUI* viewportUIPtr, ContextManager* contextManagerPtr, EventDispatcher* eventsPtr) {
    if (!actionRegPtr) return false;
    if (!viewportUIPtr) return false;
    if (!contextManagerPtr) return false;
    if (!eventsPtr) return false;
    actionRegPtr->bind(Action::CameraForward, [viewportUIPtr]() { viewportUIPtr->getCamera()->MoveForward(); });
    actionRegPtr->bind(Action::CameraBack, [viewportUIPtr]() { viewportUIPtr->getCamera()->MoveBack(); });
    actionRegPtr->bind(Action::CameraLeft, [viewportUIPtr]() { viewportUIPtr->getCamera()->MoveLeft(); });
    actionRegPtr->bind(Action::CameraRight, [viewportUIPtr]() { viewportUIPtr->getCamera()->MoveRight(); });
    actionRegPtr->bind(Action::CameraUp, [viewportUIPtr]() { viewportUIPtr->getCamera()->MoveUp(); });
    actionRegPtr->bind(Action::CameraDown, [viewportUIPtr]() { viewportUIPtr->getCamera()->MoveDown(); });
    actionRegPtr->bind(Action::SwitchControlContext, [contextManagerPtr]() { contextManagerPtr->toggleCtx(); });
    actionRegPtr->bind(Action::SaveActiveShaderFile, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::SaveActiveShaderFile, false, std::monostate{} }); });
    actionRegPtr->bind(Action::SaveProject, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::SaveProject, false, std::monostate{} }); });
    actionRegPtr->bind(Action::QuitApplication, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::Quit, false, std::monostate{} }); });
    actionRegPtr->bind(Action::FontSizeIncrease, []() { Application::increaseFont(); });
    actionRegPtr->bind(Action::FontSizeDecrease, []() { Application::decreaseFont(); });
    actionRegPtr->bind(Action::NewShaderFile, [](){});
    actionRegPtr->bind(Action::Undo, [](){});
    actionRegPtr->bind(Action::Redo, [](){});
    actionRegPtr->bind(Action::FormatActiveShader, [](){});
    actionRegPtr->bind(Action::ScreenshotViewport, [](){});
    actionRegPtr->bind(Action::FullscreenViewport, [](){});
    return true;
}

void Application::initializeUI(AppContext& ctx) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ctx.platform.initializeImGui();

    fonts[0] = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 12.0f);
    fonts[1] = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 15.0f);
    fonts[2] = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 18.0f);
    fonts[3] = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 21.0f);
    fonts[4] = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 24.0f);
    fonts[5] = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 27.0f);
    fontIdx = ctx.settings.fontIdx;
    io.FontDefault = fonts[fontIdx];

    if (!ctx.settings.styles.hasLoadedStyles) {
        ImGui::StyleColorsDark();
        ctx.settings.styles.captureFromImGui(ImGui::GetStyle());
    }
    ctx.settings.styles.applyToImGui(ImGui::GetStyle());

    ImGui_ImplOpenGL3_Init();

    ctx.settingsModal.initialize(&ctx.logger, &ctx.inputs, &ctx.keybinds, &ctx.platform, &ctx.settings);
    ctx.modals.registerModal(&ctx.settingsModal);
}

void loadPresetAssets(AppContext& ctx) {
    ctx.texture_registry.registerTexture(&ctx.preset_assets.getPresetTexture(TexturePreset::WATER));
    ctx.texture_registry.registerTexture(&ctx.preset_assets.getPresetTexture(TexturePreset::FACE));
    ctx.texture_registry.registerTexture(&ctx.preset_assets.getPresetTexture(TexturePreset::METAL));
    ctx.texture_registry.registerTexture(&ctx.preset_assets.getPresetTexture(TexturePreset::GRID));
    
    ShaderProgram* texPtr = ctx.shader_registry.getProgram("tex");
    ShaderProgram* colorPtr = ctx.shader_registry.getProgram("color");

    MeshData& plane = ctx.preset_assets.getPresetMesh(MeshPreset::PLANE);
    MeshData& cube = ctx.preset_assets.getPresetMesh(MeshPreset::CUBE);
    MeshData& pyramid = ctx.preset_assets.getPresetMesh(MeshPreset::PYRAMID);
    
    unsigned int gridID = ctx.model_cache.createModel(plane.verts, plane.indices, true, false, true);
    ctx.model_cache.setProgram(gridID, *colorPtr);
    ctx.model_cache.scaleModel(gridID, glm::vec3(5.0f));
    // ctx.model_cache.setTexture("grid", PresetAssets::getPresetTexture(TexturePreset::GRID), 0, "baseTex");

    unsigned int backpackID = ctx.model_cache.createModel("../assets/models/backpack/backpack.obj");
    ctx.model_cache.setProgram(backpackID, *texPtr);
    
    unsigned int pyramid0ID = ctx.model_cache.createModel(pyramid.verts, pyramid.indices, true, false, true);
    ctx.model_cache.setProgram(pyramid0ID, *colorPtr);
    
    ctx.model_cache.translateModel(pyramid0ID, glm::vec3(3.3f, 0.0f, -1.0f));
    ctx.model_cache.scaleModel(pyramid0ID, glm::vec3(2.0f));
    ctx.model_cache.rotateModel(pyramid0ID, 23.2f, glm::vec3(0.0f, 1.0f, 0.0f));
    
    unsigned int pyramid1ID = ctx.model_cache.createModel(pyramid.verts, pyramid.indices, true, false, true);
    ctx.model_cache.setProgram(pyramid1ID, *colorPtr);
    ctx.model_cache.translateModel(pyramid1ID, glm::vec3(-1.3f, 0.0f, -1.0f));

    unsigned int cubeID = ctx.model_cache.createModel(cube.verts, cube.indices, true, false, true);
    ctx.model_cache.setProgram(cubeID, *colorPtr);
    ctx.model_cache.translateModel(cubeID, glm::vec3(4.0f, 3.0f, -5.0f));
    ctx.model_cache.scaleModel(cubeID, glm::vec3(1.0, 0.5f, 1.0f));
    ctx.model_cache.rotateModel(cubeID, 23.2f, glm::vec3(0.5f, 0.5f, 0.5f));
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
    if (!ctx.inputs.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Input State was not initialized successfully.");
        return false;
    }
    if (!ctx.keybinds.initialize(&ctx.logger, &ctx.ctx_manager, &ctx.action_registry, &ctx.inputs, ctx.settings.keybindsMap)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Keybinds were not initialized successfully.");
        return false;
    }
    if (!ctx.platform.initialize(&ctx.logger, &ctx.ctx_manager, &ctx.keybinds, &ctx.action_registry, &ctx.inputs, ctx.app_title, &ctx.settings)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Platform layer was not initialized successfully.");
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
    if (!ctx.model_importer.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Model Importer was not initialized successfully.");
        return false;
    }
    if (!ctx.model_cache.initialize(&ctx.logger, &ctx.events, &ctx.shader_registry, &ctx.uniform_registry, &ctx.model_importer)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Model Cache was not initialized successfully.");        
        return false;
    }
    if (!ctx.inspector_engine.initialize(&ctx.logger, &ctx.shader_registry, &ctx.uniform_registry, &ctx.model_cache, &ctx.viewport_ui)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Inspector Engine was not initialized successfully.");        
        return false;
    }
    ctx.model_cache.setInspectorEnginePtr(&ctx.inspector_engine);
    if (!ctx.hot_reloader.initialize(&ctx.logger, &ctx.events, &ctx.shader_registry, &ctx.model_cache, &ctx.editor_engine, &ctx.inspector_engine)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Hot Reloader was not initialized successfully.");        
        return false;
    }
    if (!ctx.file_registry.initialize(&ctx.logger, &ctx.events, &ctx.platform)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "File Registry was not initialized successfully.");        
        return false;
    }
    if (!ctx.editor_engine.initialize(&ctx.logger, &ctx.events, &ctx.model_cache, &ctx.shader_registry, &ctx.settings.styles)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Editor Engine was not initialized successfully.");
        return false;
    }
    if (!ctx.preset_assets.initialize(&ctx.logger, &ctx.platform)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Preset Assets were not initialized successfully.");
        return false;
    }
    if (!ctx.texture_registry.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Texture Registry was not initialized successfully.");
        return false;
    }
    loadPresetAssets(ctx);
    subscribeMenuButtons(ctx);
    initializeUI(ctx);
    if (!ctx.console_ui.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Console UI was not initialized successfully.");
        return false;
    }
    if (!ctx.viewport_ui.initialize(&ctx.logger, &ctx.platform, &ctx.model_cache, &ctx.timer)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Viewport UI was not initialized successfully.");
        return false;
    }
    if (!ctx.menu_ui.initialize(&ctx.logger, &ctx.events, &ctx.modals, &ctx.keybinds)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Menu UI was not initialized successfully.");
        return false;
    }
    if (!ctx.editor_ui.initialize(&ctx.logger, &ctx.editor_engine)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Editor UI was not initialized successfully.");
        return false;
    }
    if (!ctx.inspector_ui.initialize(&ctx.logger, &ctx.inspector_engine, &ctx.texture_registry, &ctx.shader_registry, &ctx.uniform_registry, &ctx.events, &ctx.model_cache, &ctx.file_registry)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Inspector UI was not initialized successfully.");
        return false;
    }

    if (!Application::addDefaultActionBinds(&ctx.action_registry, &ctx.viewport_ui, &ctx.ctx_manager, &ctx.events)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Default actions were not bound correctly.");
        return false;
    }

    ctx.logger.addLog(LogLevel::INFO, "Application Initialization", "Application Layer Initialized.");
    Application::initialized = true;
    return true;
}

void Application::runLoop(AppContext& ctx) {
    if (!Application::initialized) {
        std::cout << "Attempting to run render loop without initializing application layer." << std::endl;
        return;
    }

    while (!Application::shouldClose(ctx)) {
        ERRLOG.printClear();
        ctx.timer.update();
        ctx.inputs.beginFrame();
        ctx.platform.pollEvents();
        ctx.platform.processInput();
        ctx.hot_reloader.update();
        ctx.events.ProcessQueue();
        Application::renderUI(ctx);
        ctx.platform.swapBuffers();
    }
}

void Application::renderUI(AppContext& ctx) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pre Render
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImFont* f = Application::fonts[Application::fontIdx];
    if (f) ImGui::PushFont(f);

    // Render UI
    ctx.inspector_ui.render();
    ctx.editor_ui.render();
    ctx.console_ui.render();
    ctx.viewport_ui.render();
    ctx.menu_ui.render();

    if (f) ImGui::PopFont();

    // Post Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::shutdown(AppContext& ctx) {
    ctx.settings.styles.captureFromImGui(ImGui::GetStyle());
    ctx.settings.fontIdx = fontIdx;
    ctx.platform.terminate();
    // UI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for (Editor* editor: ctx.editor_engine.editors) editor->destroy();
}

bool Application::shouldClose(AppContext& ctx) {
    if (!initialized) return false;
    return (ctx.platform.shouldClose() || ctx.shouldClose);
}

void Application::windowResize(AppContext& ctx, u32 _width, u32 _height) {
    ctx.settings.width = _width;
    ctx.settings.height = _height;
}
