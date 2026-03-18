#include "core/ui/FileInspectorUI.hpp"

#include <filesystem>
#include "application/SettingsStyles.hpp"
#include "core/ui/Fonts.hpp"
#include "core/EventDispatcher.hpp"
#include "core/EventTypes.hpp"
#include "core/FileRegistry.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include <string>
#include <vector>

void FileInspectorUI::draw(Logger* loggerPtr, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, EventDispatcher* eventsPtr, Fonts* fonts, SettingsStyles* styles) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->shaderTabBackgroundColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    float window_padding = styles->shaderBodyPadding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));
    if (ImGui::BeginChild("ShadersContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        float inner_padding = styles->shaderTitleInnerPadding;
        ImGui::PushFont(fonts->getL4());
        float text_height = ImGui::CalcTextSize("Shaders").y;
        float directory_height = window_padding * 2 + inner_padding * 2 + ImGui::CalcTextSize("Shaders").y;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0,0,0,0));
        if (ImGui::BeginChild("ShadersTitle", ImVec2(0, directory_height), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();

            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->shaderTitleBackgroundColor),
                styles->shaderBodyRounding,
                ImDrawFlags_RoundCornersTop
            );

            ImGui::GetWindowDrawList()->AddRect(
                p,
                ImVec2(p.x + s.x, p.y + s.y),
                ImGui::ColorConvertFloat4ToU32(styles->shaderBorderColor),
                styles->shaderBodyRounding,
                ImDrawFlags_RoundCornersTop,
                styles->shaderBorderThickness
            );

            float centerY = directory_height * 0.5f;
            float button_side = text_height - 2.0f;

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(styles->shaderTitleOffset, 0.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted("Shaders");
            
            ImGui::SetCursorPos(ImVec2(s.x - button_side- window_padding, centerY - (button_side * 0.5f)));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
            if (ImGui::Button("+##global_add", ImVec2(button_side, button_side))) {
                ImGui::OpenPopup("GlobalAddPopup");
            }
            ImGui::PopStyleVar();
            
            if (ImGui::BeginPopup("GlobalAddPopup")) {
                ImGui::PushFont(fonts->getL2());
                if (ImGui::MenuItem("Import Shader File")) {
                    eventsPtr->TriggerEvent(Event{EventType::NewFile, false,{}});
                }
                if (ImGui::MenuItem("Create Shader Program")) { 
                    static int newShaders = 0;
                    std::string newName = "myShader_" + std::to_string(newShaders);
                    
                    shaderLinkMenus.emplace(std::make_pair(newName, ShaderLinkMenu{
                        .shaderName = newName,
                        .initialized = false
                    }));
        
                    newShaders++;
                }
                ImGui::PopFont();
                ImGui::EndPopup();
            }
            ImGui::Unindent(styles->shaderTitleOffset);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopFont();

        if (ImGui::BeginChild("ShadersBody", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
            ImVec2 p = ImGui::GetWindowPos();
            ImVec2 s = ImGui::GetWindowSize();
            auto drawList = ImGui::GetWindowDrawList();

            drawList->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), 
                ImGui::ColorConvertFloat4ToU32(styles->shaderTreeBodyColor), styles->shaderBodyRounding, ImDrawFlags_RoundCornersBottom);
            
            drawList->AddRect(p, ImVec2(p.x + s.x, p.y + s.y), 
                ImGui::ColorConvertFloat4ToU32(styles->shaderBorderColor), styles->shaderBodyRounding, ImDrawFlags_RoundCornersBottom, styles->shaderBorderThickness);

            ImGui::Spacing();
            
            ImGui::Indent(window_padding);
            ImGui::TextDisabled("SHADER FILES");
            fileRegPtr->reloadMap();
            for (const auto& [fileName, fileData] : fileRegPtr->getFiles()) {
                drawStandardFileEntry(fileData, eventsPtr);
            }

            ImGui::Dummy(ImVec2(0, 20.0f));
            ImGui::Separator();

            ImGui::TextDisabled("SHADER PROGRAMS");
            drawShaderLinkMenus(shaderLinkMenus, shaderRegPtr, fileRegPtr, inspectorEngPtr);
            ImGui::Unindent(window_padding);
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void FileInspectorUI::drawRenameFileEntry(ShaderFile* fileData, EventDispatcher* eventsPtr) {
    static char buf[256];
    strncpy(buf, fileData->renameBuffer.c_str(), 255);

    bool keyboardSubmitted = ImGui::InputText(("##FileNameInput" + fileData->fileName).c_str(), buf, 256, ImGuiInputTextFlags_EnterReturnsTrue);
    fileData->renameBuffer = buf;

    ImGui::SameLine();

    bool buttonSumitted = ImGui::Button(("OK##" + fileData->fileName).c_str());

    if ((keyboardSubmitted || buttonSumitted) && !fileData->renameBuffer.empty()) {
        eventsPtr->TriggerEvent(Event{ EventType::RenameFile, false, RenameFilePayload{ fileData->fileName, fileData->renameBuffer } });
    }

    ImGui::SameLine();

    if (ImGui::Button(("CANCEL##" + fileData->fileName).c_str())) {
        fileData->state = NONE;
        fileData->renameBuffer = fileData->fileName;
    }
}

void FileInspectorUI::drawDeleteFileEntity(ShaderFile* fileData, EventDispatcher* eventsPtr) {
    ImGui::Text("%s", fileData->fileName.c_str());

    ImGui::SameLine();

    if (ImGui::Button(("DELETE##" + fileData->fileName).c_str())) {
        eventsPtr->TriggerEvent(Event{ EventType::ET_DeleteFile, false, DeleteFilePayload{ fileData->fileName } });
    }

    ImGui::SameLine();

    if (ImGui::Button(("CANCEL##" + fileData->fileName).c_str())) {
        fileData->state = NONE;
    }
}

void FileInspectorUI::drawContextMenu(ShaderFile* fileData) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Rename")) {
            fileData->state = RENAME;
        }
        if (ImGui::Selectable("Delete")) {
            fileData->state = FS_DELETE;
        }

        ImGui::EndPopup();
    }
}

void FileInspectorUI::drawStandardFileEntry(ShaderFile* fileData, EventDispatcher* eventsPtr) {
    if (ImGui::Selectable(fileData->fileName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
            eventsPtr->TriggerEvent(
                Event{
                    EventType::OpenFile,
                    false,
                    OpenFilePayload{ fileData->filePath, fileData->fileName, 0 }
                }
            );
        }
    }
    drawContextMenu(fileData);
}

void FileInspectorUI::drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, InspectorEngine* inspectorEngPtr) {
    for (const auto& [shaderName, shader] : shaderRegPtr->getPrograms()) {
        if (!menus.contains(shaderName)) {
            menus[shaderName] = ShaderLinkMenu{
                .shaderName = shaderName,
                .initialized = false,
            };
        }
    }

    std::vector<std::string> vertChoices{ "" };
    std::vector<std::string> fragChoices{ "" };
    std::vector<std::string> geoChoices{ "" };

    for (const auto& [id, file] : fileRegPtr->getFiles()) {
        std::string& extension = file->extension;
        std::string& filePath = file->filePath;
        if (extension == ".vert") {
            vertChoices.push_back(filePath);
        } else if (extension == ".frag") {
            fragChoices.push_back(filePath);
        }
    }

    std::vector<const char*> vertChoicesC;
    std::vector<const char*> fragChoicesC;
    std::vector<const char*> geoChoicesC;
    vertChoicesC.reserve(vertChoices.size());
    fragChoicesC.reserve(fragChoices.size());
    geoChoicesC.reserve(geoChoices.size());
    for (size_t i = 0; i < vertChoices.size(); i++) {
        vertChoicesC.push_back(vertChoices[i].c_str());
    }
    for (size_t i = 0; i < fragChoices.size(); i++) {
        fragChoicesC.push_back(fragChoices[i].c_str());
    }
    for (size_t i = 0; i < geoChoices.size(); i++) {
        geoChoicesC.push_back(geoChoices[i].c_str());
    }

    ImGuiID guiID = 0;
    for (auto& [shaderName, menu] : menus) {
        if (!menu.initialized) {
            initializeMenu(menu, vertChoicesC, geoChoicesC, fragChoicesC, shaderRegPtr);
        }

        if (ImGui::TreeNode(menu.shaderName.c_str())) {
            ImGui::PushID(guiID);
            drawShaderLinkMenu(menu, vertChoicesC, geoChoicesC, fragChoicesC, inspectorEngPtr);
            ImGui::PopID();
            guiID++;
            ImGui::TreePop();
        }
    }
}

void FileInspectorUI::initializeMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices, ShaderRegistry* shaderRegPtr) {
    const ShaderProgram* oldProgram = shaderRegPtr->getProgram(menu.shaderName);
    bool isNewProgram = oldProgram == nullptr;
    if (isNewProgram) {
        menu.vertSelection = 0;
        menu.fragSelection = 0;
        menu.geometrySelection = 0;
        menu.initialized = true;
        return;
    }

    auto getNormalizedPath = [](const std::string& p) -> std::string {
        if (p.empty()) return "";
        try {
            return std::filesystem::weakly_canonical(p).string();
        } catch (...) {
            return p;
        }
    };
    std::string registryVert = getNormalizedPath(oldProgram->vertPath);
    std::string registryFrag = getNormalizedPath(oldProgram->fragPath);

    for (size_t i = 0; i < vertChoices.size(); i++) {
        if (i > 0 && vertChoices[i] != nullptr) {
            if (getNormalizedPath(vertChoices[i]) == registryVert) {
                menu.vertSelection = (int)i;
                break;
            }
        }
    }
    for (size_t i = 0; i < fragChoices.size(); i++) {
        if (i > 0 && fragChoices[i] != nullptr) {
            if (getNormalizedPath(fragChoices[i]) == registryFrag) {
                menu.fragSelection = (int)i;
                break;
            }
        }
    }
    menu.geometrySelection = 0;
    menu.initialized = true;
}

void FileInspectorUI::drawShaderLinkMenu(ShaderLinkMenu& menu, const std::vector<const char*>& vertChoices, const std::vector<const char*>& geoChoices, const std::vector<const char*>& fragChoices, InspectorEngine* inspectorEngPtr) {
    bool changed = false;
    ShaderLinkMenu oldMenu = menu;

    if (ImGui::Combo("Vertex Shader", &menu.vertSelection, vertChoices.data(), (int)vertChoices.size())) {
        changed = true;
    }
    if (ImGui::Combo("Geometry Shader", &menu.geometrySelection, geoChoices.data(), (int)geoChoices.size())) {
        changed = true;
    }
    if (ImGui::Combo("Fragment Shader", &menu.fragSelection, fragChoices.data(), (int)fragChoices.size())) {
        changed = true;
    }

    bool validSelection = menu.fragSelection != 0 && menu.vertSelection != 0 && menu.shaderName != "";
    if (validSelection) {
        ImGui::Text("Valid");
    } else {
        ImGui::Text("Invalid! Using old program...");
    }
    if (validSelection && changed) {
        const std::string vert = vertChoices[menu.vertSelection];
        const std::string frag = fragChoices[menu.fragSelection];
        const std::string& name = menu.shaderName;
        if (!inspectorEngPtr->handleEditShaderProgram(vert, frag, name)) {
            menu = oldMenu;
        }
    }
}
