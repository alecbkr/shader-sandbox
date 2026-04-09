#pragma once

#include <filesystem>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <limits>

class ShaderFile;

struct ShaderLinkMenu {
    std::string shaderName;
    int vertSelection;
    int geometrySelection;
    int fragSelection;
    bool initialized;
};

struct ShaderLinkMenuChoices {
    const std::vector<const char*>& vertChars;
    const std::vector<const char*>& geoChars;
    const std::vector<const char*>& fragChars;
    const std::vector<const ShaderFile*>& vertFiles;
    const std::vector<const ShaderFile*>& geoFiles;
    const std::vector<const ShaderFile*>& fragFiles;
};


class Logger;
class InspectorEngine;
class ShaderRegistry;
class FileRegistry;
class EventDispatcher;
class ShaderProgram;
class Fonts;
struct SettingsStyles;

class FileInspectorUI {
public:
    void draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, EventDispatcher* eventsPtr, Fonts* fonts, SettingsStyles* styles);

private:
    std::unordered_map<std::string, ShaderLinkMenu> shaderLinkMenus;

    void drawRenameFileEntry(ShaderFile* fileData, EventDispatcher* eventsPtr);
    void drawDeleteFileEntity(ShaderFile* fileData, EventDispatcher* eventsPtr);
    void drawContextMenu(ShaderFile* fileData, EventDispatcher* evnetsPtr);
    void drawStandardFileEntry(ShaderFile* fileData, EventDispatcher* eventsPtr);
    void drawPresetShaderEntry(std::filesystem::path filePath, EventDispatcher* eventsPtr);
    // void drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, InspectorEngine* inspectorEngPtr);
    void drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, InspectorEngine* inspectorEngPtr, SettingsStyles* styles);
    void drawShaderLinkMenu(ShaderLinkMenu& menu,ShaderLinkMenuChoices& choices, InspectorEngine* inspectorEngPtr);
    void initializeMenu(ShaderLinkMenu& menu, ShaderLinkMenuChoices& choices, ShaderRegistry* shaderRegPtr);
    void drawShaderProgramCard(ShaderLinkMenu& menu, ShaderLinkMenuChoices& choices, InspectorEngine* inspectorEngPtr, SettingsStyles* styles, ImGuiID guiID);
    bool showPresets = true;
    bool newProgram = false;
    char newProgramBuf[256] = "";
    Logger* loggerPtr = nullptr;
};
