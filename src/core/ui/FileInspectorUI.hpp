#pragma once

#include "core/FileRegistry.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>
#include <unordered_map>
#include <vector>

struct ShaderLinkMenu {
    std::string shaderName;
    int vertSelection;
    int geometrySelection;
    int fragSelection;
    bool initialized;
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
    void drawContextMenu(ShaderFile* fileData);
    void drawStandardFileEntry(ShaderFile* fileData, EventDispatcher* eventsPtr);
    void drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, InspectorEngine* inspectorEngPtr);
    void drawShaderLinkMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices, InspectorEngine* inspectorEngPtr);
    void initializeMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices, ShaderRegistry* shaderRegPtr);
};
