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
#include "engine/Errorlog.hpp"
#include "persistence/ProjectLoader.hpp"
#include "core/ui/themes/default.hpp"
#include "object/AssimpImporter.hpp"

bool Application::initialized = false;

void Application::addSubscriptions(AppContext& ctx) {
    ctx.events.Subscribe(EventType::ContextSwitch, [&ctx](const EventPayload&) -> bool {
        ctx.ctx_manager.toggleCtx();
        if (ctx.ctx_manager.isCamera()) ctx.platform.setCursorStatus(false);
        else ctx.platform.setCursorStatus(true);
        return true;
    });
    ctx.events.Subscribe(EventType::NewProject, [&ctx](const EventPayload&) -> bool {
        ProjectLoader::save(ctx.project, &ctx.model_cache, &ctx.material_cache);
        ctx.settings.projectToOpen = "";
        ctx.projectSwitch = true;
        return false;
    });
    ctx.events.Subscribe(EventType::SaveProject, [&ctx](const EventPayload&) -> bool {
        if (ctx.project.previouslySaved) ProjectLoader::save(ctx.project, &ctx.model_cache, &ctx.material_cache);
        else ctx.modals.open(SaveAsModal::ID);
        ctx.logger.addLog(LogLevel::INFO, "ProjectLoader", "Project Saved");
        return false;
    });
    ctx.events.Subscribe(EventType::RenameProject, [&ctx](const EventPayload&) -> bool {
        ctx.project.previouslySaved = false;
        ctx.modals.open(SaveAsModal::ID);
        return false;
    });
    ctx.events.Subscribe(EventType::CloneFile, [&ctx](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<CloneFilePayload>(&payload)) {
            std::filesystem::path sourcePath(data->fileName);
            if (!std::filesystem::exists(sourcePath)) return false;
            std::string originalName = sourcePath.filename().string();
            std::string newFileName = findNextFileNumber(ctx.project.projectShadersDir, originalName);
            std::filesystem::path destPath = ctx.project.projectShadersDir / newFileName;
            std::filesystem::copy(sourcePath, destPath);
            ctx.file_registry.reloadMap(); 
            return true;
        }
        return false;
    });
    ctx.events.Subscribe(EventType::Quit, [&ctx](const EventPayload&) -> bool {
        ctx.shouldClose = true;
        return true;
    });
    ctx.events.Subscribe(EventType::LoadModel, [&ctx](const EventPayload& payload) -> bool {
        if (const auto* data = std::get_if<LoadModelPayload>(&payload)) {
            unsigned int newModelID = ctx.assimp_importer.importModel(data->filePath); 
            if (newModelID != INVALID_MODEL_ID) {
                ctx.logger.addLog(LogLevel::INFO, "InspectorObject", "Successfully loaded model: " + data->filePath);
                ctx.inspector_engine.refreshUniforms();
            } else {
                ctx.logger.addLog(LogLevel::LOG_ERROR, "Application", "Failed to import model: " + data->filePath); 
            }
            return true; 
        }
        return false; 
    });  
}

bool Application::addDefaultActionBinds(ActionRegistry* actionRegPtr, ViewportUI* viewportUIPtr, ContextManager* contextManagerPtr, EventDispatcher* eventsPtr, Fonts* fontsPtr) {
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
    actionRegPtr->bind(Action::SwitchControlContext, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::ContextSwitch, false, std::monostate{} }); });
    actionRegPtr->bind(Action::SaveActiveShaderFile, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::SaveActiveShaderFile, false, std::monostate{} }); });
    actionRegPtr->bind(Action::SaveProject, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::SaveProject, false, std::monostate{} }); });
    actionRegPtr->bind(Action::QuitApplication, [eventsPtr]() { eventsPtr->TriggerEvent({ EventType::Quit, false, std::monostate{} }); });
    actionRegPtr->bind(Action::FontSizeIncrease, [fontsPtr]() { fontsPtr->increaseFont(); });
    actionRegPtr->bind(Action::FontSizeDecrease, [fontsPtr]() { fontsPtr->decreaseFont(); });
    actionRegPtr->bind(Action::NewShaderFile, [](){});
    actionRegPtr->bind(Action::Undo, [](){});
    actionRegPtr->bind(Action::Redo, [](){});
    actionRegPtr->bind(Action::FormatActiveShader, [](){});
    actionRegPtr->bind(Action::ScreenshotViewport, [](){});
    actionRegPtr->bind(Action::FullscreenViewport, [](){});
    actionRegPtr->bind(Action::MouseMove, [viewportUIPtr]() { viewportUIPtr->getCamera()->ProcessMouseMovement(); });
    actionRegPtr->bind(Action::EditorFind, [eventsPtr](){ eventsPtr->TriggerEvent({ EventType::ToggleEditorFind, false, std::monostate{} }); });
    return true;
}

void Application::initializeUI(AppContext& ctx) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ctx.fonts.initialize(ctx.settings.fontIdx);

    if (!ctx.settings.styles.hasLoadedStyles) {
        ImGui::StyleColorsDark();
        ctx.settings.styles.captureFromImGui(ImGui::GetStyle());
    }
    ctx.settings.styles.applyToImGui(ImGui::GetStyle());

    ImGui_ImplGlfw_InitForOpenGL(ctx.platform.getWindow().getGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init();

    ctx.settingsModal.initialize(&ctx.logger, &ctx.inputs, &ctx.keybinds, &ctx.platform, &ctx.settings);
    ctx.saveAsModal.initialize(&ctx.logger, &ctx.project, &ctx.events, &ctx.settings, &ctx.projectSwitch);
    ctx.openProjectModal.initialize(&ctx.project, &ctx.settings, &ctx.model_cache, &ctx.material_cache, &ctx.projectSwitch);
    ctx.addObjectModal.initialize(&ctx.model_cache, &ctx.inspector_engine, &ctx.project, &ctx.events);
    ctx.addTextureModal.initialize(&ctx.texture_cache, ctx.project.projectAssetsDir);
    ctx.modals.registerModal(&ctx.settingsModal);
    ctx.modals.registerModal(&ctx.saveAsModal);
    ctx.modals.registerModal(&ctx.openProjectModal);
    ctx.modals.registerModal(&ctx.addObjectModal); 
    ctx.modals.registerModal(&ctx.addTextureModal);

    if (!ctx.settings.settingsFound) {
        DefaultTheme::applyDefaultTheme(ctx.settings.styles);
    }
}

void Application::loadDefaultScene(AppContext& ctx) {
    // ctx.shader_registry.registerProgram(ctx.project.projectShadersDir / "color.vert", ctx.project.projectShadersDir / "color.frag", "color");

    // unsigned int skyMatID = ctx.material_cache.createBlankMaterial();
    // ctx.material_cache.addTextureToMaterial(skyMatID, "../assets/textures/skybox", true);
    // ctx.material_cache.getMaterial(skyMatID)->setProgramID("skybox");

    // unsigned int skyboxID = ctx.model_cache.createPreset(ModelType::CubePreset);
    // ctx.model_cache.changeModelMaterial(skyboxID, skyMatID);
    // ctx.model_cache.setAsSkybox(skyboxID);


    // ---TESTS---
    // ctx.assimp_importer.importModel("../assets/models/backpack/backpack.obj");


    // ---END TESTS---

    // ctx.inspector_engine.refreshUniforms();
}

bool Application::initialize(AppContext& ctx) {
    if (Application::initialized) {
        ctx.logger.addLog(LogLevel::WARNING, "Application Initialization", "Application was already initialized.");
        return false;
    }
    if (!ctx.logger.initialize(ctx.app_title, ctx.project.projectTitle)) {
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
    if (!ctx.shader_registry.initialize(&ctx.logger, &ctx.project)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Shader Registry was not initialized successfully.");
        return false;
    }
    if (!ctx.uniform_registry.initialize(&ctx.logger, &ctx.project)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Uniform Registry was not initialized successfully.");
        return false;
    }
    if (!ctx.model_cache.initialize(&ctx.logger, &ctx.events, &ctx.preset_assets)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Model Cache was not initialized successfully.");
        return false;
    }
    if (!ctx.material_cache.initialize(&ctx.logger, &ctx.events, &ctx.texture_cache, &ctx.model_cache, &ctx.uniform_registry, &ctx.shader_registry, ctx.project.previouslySaved)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Material Cache was not initialized successfully.");
        return false;
    }
    if (!ctx.texture_cache.initialize(&ctx.logger)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Texture Cache was not initialized successfully.");
        return false;
    }
    if (!ctx.inspector_engine.initialize(&ctx.logger, &ctx.shader_registry, &ctx.uniform_registry, &ctx.model_cache, &ctx.viewport_ui, &ctx.material_cache, &ctx.platform)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Inspector Engine was not initialized successfully.");
        return false;
    }
    if (!ctx.console_engine.initialize(&ctx.logger, ctx.project.consoleSettings)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Model Importer was not initialized successfully.");
        return false;
    }
    if (!ctx.hot_reloader.initialize(&ctx.logger, &ctx.events, &ctx.shader_registry, &ctx.model_cache, &ctx.editor_engine, &ctx.inspector_engine, &ctx.ctx_manager)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Hot Reloader was not initialized successfully.");
        return false;
    }
    if (!ctx.file_registry.initialize(&ctx.logger, &ctx.events, &ctx.platform, &ctx.project)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "File Registry was not initialized successfully.");
        return false;
    }
    if (!ctx.editor_engine.initialize(&ctx.logger, &ctx.events, &ctx.model_cache, &ctx.shader_registry, &ctx.settings.styles, &ctx.project)) {
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
    ctx.inspector_engine.refreshUniforms();
    initializeUI(ctx);
    if (!ctx.console_ui.initialize(&ctx.logger, &ctx.console_engine, &ctx.settings.styles, &ctx.fonts)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Console UI was not initialized successfully.");
        return false;
    }
    if (!ctx.menu_ui.initialize(&ctx.logger, &ctx.platform, &ctx.events, &ctx.modals, &ctx.keybinds, &ctx)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Menu UI was not initialized successfully.");
        return false;
    }
    if (!ctx.editor_ui.initialize(&ctx.logger, &ctx.editor_engine, &ctx.ctx_manager, &ctx.events, &ctx.project, &ctx.fonts)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Editor UI was not initialized successfully.");
        return false;
    }
    if (!ctx.inspector_ui.initialize(&ctx.logger, &ctx.inspector_engine, &ctx.texture_registry, &ctx.texture_cache, &ctx.shader_registry, &ctx.uniform_registry, &ctx.events, &ctx.model_cache, &ctx.file_registry, &ctx.material_cache, &ctx.fonts, &ctx.project, &ctx.settings.styles, &ctx.modals)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Inspector UI was not initialized successfully.");
        return false;
    }
    if (!ctx.renderer.initialize(&ctx.logger, &ctx.events, &ctx.model_cache, &ctx.material_cache, &ctx.texture_cache, &ctx.shader_registry, &ctx.uniform_registry, &ctx.inspector_engine)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Renderer was not initialized successfully.");
        return false;
    }
    ctx.material_cache.initializeAfterRenderer(&ctx.renderer, &ctx.inspector_engine);
    if (!ctx.viewport_ui.initialize(&ctx.logger, &ctx.platform, &ctx.renderer, &ctx.timer, &ctx.inputs)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Viewport UI was not initialized successfully.");
        return false;
    }
    if (!Application::addDefaultActionBinds(&ctx.action_registry, &ctx.viewport_ui, &ctx.ctx_manager, &ctx.events, &ctx.fonts)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Default actions were not bound correctly.");
        return false;
    }
    if (!ctx.assimp_importer.initialize(&ctx.logger, &ctx.model_cache, &ctx.material_cache, &ctx.inspector_engine, &ctx.project)) {
        ctx.logger.addLog(LogLevel::CRITICAL, "Application Initialization", "Model Cache was not initialized successfully.");
        return false;
    }
    addSubscriptions(ctx);

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
        ctx.timer.update();
        ctx.inputs.beginFrame();
        ctx.platform.pollEvents();
        ctx.platform.processInput();
        ctx.hot_reloader.update();
        ctx.events.ProcessQueue();
        Application::renderUI(ctx);
        ctx.platform.swapBuffers();
    }

    glfwTerminate();
}

void Application::renderUI(AppContext& ctx) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pre Render
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImFont* f = ctx.fonts.getL1();
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
    ctx.editor_engine.shutdown();

    ctx.settings.styles.captureFromImGui(ImGui::GetStyle());
    ctx.settings.fontIdx = ctx.fonts.getFontIndex();
    if (initialized) ctx.project.consoleSettings = ctx.console_engine.getToggles();

    ctx.platform.terminate();

    // UI Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    for (Editor* editor: ctx.editor_engine.editors) editor->destroy();

    initialized = false;
}

bool Application::shouldClose(AppContext& ctx) {
    if (!initialized) return false;
    return (ctx.platform.shouldClose() || ctx.shouldClose || ctx.projectSwitch);
}

void Application::windowResize(AppContext& ctx, u32 _width, u32 _height) {
    ctx.settings.width = _width;
    ctx.settings.height = _height;
}

std::string Application::findNextFileNumber(const std::filesystem::path& baseFolder, const std::string& startingName) {
    int i = 1;
    std::filesystem::path p(startingName);
    std::string name = p.stem().string();
    std::string extension = p.extension().string();

    if (!std::filesystem::exists(baseFolder / startingName)) {
        return startingName;
    }

    while (true) {
        std::string newName = name + "(" + std::to_string(i) + ")" + extension;
        
        if (!std::filesystem::exists(baseFolder / newName)) {
            return newName;
        }
        i++;
    }
}
