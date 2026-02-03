#pragma once
#include <types.hpp>
#include "core/logging/Logger.hpp"
#include "core/input/ActionRegistry.hpp"
#include "core/input/ContextManager.hpp"
#include "core/input/Keybinds.hpp"
#include "platform/Platform.hpp"
#include "core/input/InputState.hpp"
#include "engine/AppTimer.hpp"
#include "core/EventDispatcher.hpp"
#include "core/ShaderRegistry.hpp"
#include "core/UniformRegistry.hpp"
#include "core/EditorEngine.hpp"
#include "object/ModelCache.hpp"
#include "core/InspectorEngine.hpp"
#include "core/HotReloader.hpp"
#include "core/FileRegistry.hpp"
#include "core/EditorEngine.hpp"
#include "presets/PresetAssets.hpp"
#include "core/TextureRegistry.hpp"
#include "core/ui/ConsoleUI.hpp"
#include "core/ui/ViewportUI.hpp"
#include "core/ui/MenuUI.hpp"
#include "core/ui/EditorUI.hpp"
#include "core/ui/InspectorUI.hpp"

struct AppContext {
    AppContext(u32 _width, u32 _height, const char* _app_title)
        : width(_width), height(_height), app_title(_app_title) {};

    u32 width;
    u32 height;
    const char* app_title;

    Logger logger;
    ActionRegistry action_registry;
    ContextManager ctx_manager;
    Keybinds keybinds;
    Platform platform;
    InputState inputs;
    AppTimer timer;
    EventDispatcher events;
    ShaderRegistry shader_registry;
    UniformRegistry uniform_registry;
    ModelCache model_cache;
    InspectorEngine inspector_engine;
    HotReloader hot_reloader;
    FileRegistry file_registry;
    EditorEngine editor_engine;
    PresetAssets preset_assets;
    TextureRegistry texture_registry;
    ConsoleUI console_ui;
    ViewportUI viewport_ui;
    MenuUI menu_ui;
    EditorUI editor_ui;
    InspectorUI inspector_ui;
};