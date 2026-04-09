#include "core/ui/FileInspectorUI.hpp"

#include "application/SettingsStyles.hpp"
#include "core/logging/LogSink.hpp"
#include "core/ui/Fonts.hpp"
#include "core/EventDispatcher.hpp"
#include "core/EventTypes.hpp"
#include "core/FileRegistry.hpp"
#include "core/InspectorEngine.hpp"
#include "core/ShaderRegistry.hpp"
#include "engine/ShaderProgram.hpp"
#include "core/logging/Logger.hpp"
#include <string>
#include <vector>

void FileInspectorUI::draw(Logger* loggerPtr_, InspectorEngine* inspectorEngPtr, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, EventDispatcher* eventsPtr, Fonts* fonts, SettingsStyles* styles) {
    loggerPtr = loggerPtr_;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->shaderTabBackgroundColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    float window_padding = styles->shaderBodyPadding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding));
    if (ImGui::BeginChild("ShadersContent", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding)) {
        float inner_padding = styles->shaderTitleInnerPadding;
        ImGui::PushFont(fonts->getL4());
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

            ImGui::SetCursorPosY(inner_padding + window_padding);
            ImGui::Dummy(ImVec2(styles->shaderTitleOffset, 0.0f));
            ImGui::SameLine();
            ImGui::TextUnformatted("Shaders");

            ImVec2 plusSize = ImGui::CalcTextSize("+");
            float buttonPaddingX = 6.5f;

            float boxWidth = plusSize.x + (buttonPaddingX * 2.0f);
            float boxHeight = plusSize.y;

            ImGui::SameLine(ImGui::GetContentRegionAvail().x - boxWidth);

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.6f, 0.6f)); 
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);

            if (ImGui::Button("+##global_add", ImVec2(boxWidth, boxHeight))) {
                ImGui::OpenPopup("GlobalAddPopup");
            }
            ImGui::PopStyleVar(3);

            if (ImGui::BeginPopup("GlobalAddPopup")) {
                ImGui::PushFont(fonts->getL2());
                if (ImGui::MenuItem("New shader file")) {
                    eventsPtr->TriggerEvent(Event{EventType::NewFile, false,{}});
                }
                if (ImGui::MenuItem("New shader program")) {
                    newProgram = true;
                    strncpy(newProgramBuf, "NewProgram", 255);
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
            fileRegPtr->reloadMap();
            const auto& files = fileRegPtr->getFiles();
            if (!files.empty()) {
                ImGui::PushFont(fonts->getL3());
                ImGui::TextDisabled("Files");
                ImGui::PopFont(); 
                for (const auto& [fileName, fileData] : files) {
                    switch (fileData->state) {
                        case RENAME:
                            drawRenameFileEntry(fileData, eventsPtr);
                            break;
                        case FS_DELETE:
                            drawDeleteFileEntity(fileData, eventsPtr);
                            break;
                        case NEW:
                            break;
                        case NONE:
                        default:
                            drawStandardFileEntry(fileData, eventsPtr);
                            break;
                    }
                }
            }
            ImGui::Dummy(ImVec2(0, 20.0f));

            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            if (ImGui::Selectable("Presets##Header", false, ImGuiSelectableFlags_AllowOverlap)) {
                showPresets = !showPresets;
            }

            ImGui::SameLine();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0,0,0,0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0,0,0,0));
            if (ImGui::ArrowButton("##ToggleArrow", showPresets ? ImGuiDir_Down : ImGuiDir_Right)) {
                showPresets = !showPresets;
            }
            ImGui::PopStyleColor(3);
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            if (showPresets) {
                for (const auto& filePath : fileRegPtr->getPresetShaders()) {
                    drawPresetShaderEntry(filePath, eventsPtr);
                }
            }
            ImGui::Dummy(ImVec2(0, 20.0f));
            const auto& programs = shaderRegPtr->getPrograms();
            if (newProgram || !programs.empty() || !shaderLinkMenus.empty()){
                ImGui::TextDisabled("Programs");
                if (newProgram) {
                    bool submitted = ImGui::InputText("##NewProgramInput", newProgramBuf, 256, ImGuiInputTextFlags_EnterReturnsTrue);
                    
                    ImGui::SameLine();
                    if (ImGui::Button("OK") || submitted) {
                        std::string newName = newProgramBuf;
                        if (newName.empty()) {
                            loggerPtr->addLog(LogLevel::LOG_ERROR, "UI", "Program name cannot be empty.");
                        } else {
                            bool repeatName = false;
                            for (auto& [ID, menu] : shaderLinkMenus) {
                                if (menu.shaderName == newName) {
                                    repeatName = true;
                                    loggerPtr->addLog(LogLevel::LOG_ERROR, "UI", "Program \"" + newName + "\" already exists!");
                                    break;
                                }
                            }
                            if (!repeatName) {
                                shaderLinkMenus.emplace(newName, ShaderLinkMenu{
                                    .shaderName = newName,
                                    .initialized = false
                                });
                            }
                            newProgram = false; 
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel")) {
                        newProgram = false;
                    }
                }
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 10.0f));
                drawShaderLinkMenus(shaderLinkMenus, shaderRegPtr, fileRegPtr, inspectorEngPtr, styles);
                ImGui::Unindent(window_padding);
                ImGui::PopStyleVar(); 
            }
            
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    loggerPtr_ = nullptr;
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

void FileInspectorUI::drawContextMenu(ShaderFile* fileData, EventDispatcher* eventsPtr) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Rename")) {
            fileData->state = RENAME;
        }
        if (ImGui::Selectable("Delete")) {
            fileData->state = FS_DELETE;
        }
        if (ImGui::Selectable("Clone")) {
            eventsPtr->TriggerEvent(CloneFileEvent(fileData->filePath)); 
        }

        ImGui::EndPopup();
    }
}

void FileInspectorUI::drawStandardFileEntry(ShaderFile* fileData, EventDispatcher* eventsPtr) {
    std::string uniqueID = fileData->fileName + "##" + fileData->filePath;
    bool isSelected = ImGui::Selectable(uniqueID.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
    if (isSelected && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        eventsPtr->TriggerEvent(
            Event{
                EventType::OpenFile,
                false,
                OpenFilePayload{ fileData->filePath, fileData->fileName, false }
            }
        );
    }
    drawContextMenu(fileData, eventsPtr);
}

void FileInspectorUI::drawPresetShaderEntry(std::filesystem::path filePath, EventDispatcher* eventsPtr) {
    std::string uniqueID = filePath.filename().string() + "##preset" + filePath.string();
    bool isSelected = ImGui::Selectable(uniqueID.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
    if (isSelected && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        eventsPtr->TriggerEvent(
            Event{
                EventType::OpenFile,
                false,
                OpenFilePayload{ filePath.string(), filePath.filename().string(), true }
            }
        );
    }
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Clone Shader File")) {
            eventsPtr->TriggerEvent(CloneFileEvent(filePath.string()));
        }
        ImGui::EndPopup();
    }
}

void FileInspectorUI::drawShaderLinkMenus(std::unordered_map<std::string, ShaderLinkMenu>& menus, ShaderRegistry* shaderRegPtr, FileRegistry* fileRegPtr, InspectorEngine* inspectorEngPtr, SettingsStyles* styles) {
    for (const auto& [ID, shader] : shaderRegPtr->getPrograms()) {
        if (!menus.contains(shader->name)) {
            menus[shader->name] = ShaderLinkMenu{
                .shaderName = shader->name,
                .initialized = false,
            };
        }
    }

    std::vector<const ShaderFile*> vertFiles{ nullptr };
    std::vector<const ShaderFile*> fragFiles{ nullptr };
    std::vector<const ShaderFile*> geoFiles{ nullptr };

    for (const auto& [id, file] : fileRegPtr->getFiles()) {
        std::string& extension = file->extension;
        if (extension == ".vert") {
            vertFiles.push_back(file);
        } else if (extension == ".frag") {
            fragFiles.push_back(file);
        }
    }

    std::vector<const char*> vertChars;
    std::vector<const char*> fragChars;
    std::vector<const char*> geoChars;
    vertChars.reserve(vertFiles.size());
    fragChars.reserve(fragFiles.size());
    geoChars.reserve(geoFiles.size());
    for (size_t i = 0; i < vertFiles.size(); i++) {
        const ShaderFile* choice = vertFiles[i];
        if (choice != nullptr) {
            vertChars.push_back(vertFiles[i]->fileName.c_str());
        }
        else {
            vertChars.push_back("");
        }
    }
    for (size_t i = 0; i < fragFiles.size(); i++) {
        const ShaderFile* choice = fragFiles[i];
        if (choice != nullptr) {
            fragChars.push_back(fragFiles[i]->fileName.c_str());
        }
        else {
            fragChars.push_back("");
        }
    }
    for (size_t i = 0; i < geoFiles.size(); i++) {
        const ShaderFile* choice = geoFiles[i];
        if (choice != nullptr) { geoChars.push_back(geoFiles[i]->fileName.c_str()); } else { geoChars.push_back("");
        }
    }

    ImGuiID guiID = 0;

    ShaderLinkMenuChoices choices{
        .vertChars = vertChars,
        .geoChars = geoChars,
        .fragChars = fragChars,
        .vertFiles = vertFiles,
        .geoFiles = geoFiles,
        .fragFiles = fragFiles
    };

    size_t menuCount = menus.size(); 
    size_t currIdx = 0; 

    
    for (auto& [shaderName, menu] : menus) {
        if (!menu.initialized) {
            initializeMenu(menu, choices, shaderRegPtr);
        }

        drawShaderProgramCard(menu, choices, inspectorEngPtr, styles, guiID);
        guiID++;
        currIdx++;
        
        if (currIdx < menuCount) {
            ImGui::Dummy(ImVec2(0.0f, 2.0f));
        }

        // if (ImGui::TreeNode(menu.shaderName.c_str())) {
        //     ImGui::PushID(guiID);
        //     drawShaderLinkMenu(menu, choices, inspectorEngPtr);
        //     ImGui::PopID();
        //     guiID++;
        //     ImGui::TreePop();
        // }
    }
}

void FileInspectorUI::initializeMenu(ShaderLinkMenu& menu, ShaderLinkMenuChoices& choices, ShaderRegistry* shaderRegPtr) {
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

    for (size_t i = 0; i < choices.vertFiles.size(); i++) {
        if (i > 0 && choices.vertFiles[i] != nullptr) {
            if (getNormalizedPath(choices.vertFiles[i]->filePath) == registryVert) {
                menu.vertSelection = (int)i;
                break;
            }
        }
    }
    for (size_t i = 0; i < choices.fragFiles.size(); i++) {
        if (i > 0 && choices.fragFiles[i] != nullptr) {
            if (getNormalizedPath(choices.fragFiles[i]->filePath) == registryFrag) {
                menu.fragSelection = (int)i;
                break;
            }
        }
    }
    menu.geometrySelection = 0;
    menu.initialized = true;
}

void FileInspectorUI::drawShaderLinkMenu(ShaderLinkMenu& menu, ShaderLinkMenuChoices& choices, InspectorEngine* inspectorEngPtr, SettingsStyles *styles) {
    bool changed = false;
    ShaderLinkMenu oldMenu = menu;
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));  
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 4.0f)); 
 
    if (ImGui::Combo("Vertex Shader", &menu.vertSelection, choices.vertChars.data(), (int)choices.vertChars.size())) {
        changed = true;
    }

    /*
    ImGui::Dummy(ImVec2(0.0f, 4.0f)); 
    if (ImGui::Combo("Geometry Shader", &menu.geometrySelection, choices.geoChars.data(), (int)choices.geoChars.size())) {
        changed = true;
    }
    */
    
    ImGui::Dummy(ImVec2(0.0f, 8.0f)); 

    if (ImGui::Combo("Fragment Shader", &menu.fragSelection, choices.fragChars.data(), (int)choices.fragChars.size())) {
        changed = true;
    }

    ImGui::Dummy(ImVec2(0.0f, 15.0f)); 
    bool validSelection = menu.fragSelection != 0 && menu.vertSelection != 0 && menu.shaderName != "";
    if (validSelection) {
        ImGui::PushStyleColor(ImGuiCol_Text, styles->consoleInfoColor); 
        ImGui::Text("Valid. Program Linked");
        ImGui::PopStyleColor(); 
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, styles->consoleErrorColor); 
        ImGui::Text("Invalid! Using old program...");
        ImGui::PopStyleColor(); 
    }

    ImGui::PopStyleVar(2);


    if (validSelection && changed) {
        const ShaderFile* vertFile = choices.vertFiles[menu.vertSelection];
        const ShaderFile* fragFile = choices.fragFiles[menu.fragSelection];
        if (vertFile == nullptr || fragFile == nullptr) {
            if (loggerPtr != nullptr) {
                loggerPtr->addLog(LogLevel::LOG_ERROR, "FileInspectorUI::drawShaderLinkMenu", "user selected null file!");
            }
            else {
                std::cerr << "loggerPtr is null in FileInspectorUI!!!" << std::endl;
            }
        }
        const std::string vertPath = vertFile->filePath;
        const std::string fragPath = fragFile->filePath;
        const std::string& name = menu.shaderName;
        if (!inspectorEngPtr->handleEditShaderProgram(vertPath, fragPath, name)) {
            menu = oldMenu;
        }
    }


}

void FileInspectorUI::drawShaderProgramCard(ShaderLinkMenu& menu, ShaderLinkMenuChoices& choices, InspectorEngine* inspectorEngPtr, SettingsStyles* styles, ImGuiID guiID) {
    ImGui::PushID(guiID);

    ImVec4 headerColor = styles->shaderTitleBackgroundColor;
    ImVec4 headerHovered = ImVec4(headerColor.x * 1.15f, headerColor.y * 1.15f, headerColor.z * 1.15f, headerColor.w);
    ImVec4 headerActive = headerColor;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, styles->shaderTreeBodyColor); 
    ImGui::PushStyleColor(ImGuiCol_Border, styles->shaderBorderColor);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, styles->shaderBodyRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, styles->shaderBorderThickness);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGuiChildFlags shaderCardFlags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders; 

    if (ImGui::BeginChild(("ShaderCard##" + menu.shaderName).c_str(), ImVec2(0, 0), shaderCardFlags)) {
        
        ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHovered);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerActive);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, styles->shaderTitleInnerPadding + 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f));

        bool isExpanded = ImGui::TreeNodeEx(
            menu.shaderName.c_str(),
            ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed
        );

        ImGui::PopStyleVar(3);

        if (isExpanded) {
            ImGui::Dummy(ImVec2(0, 0.5f));
            
            ImGui::PushStyleColor(ImGuiCol_Separator, styles->shaderBorderColor);
            ImGui::Separator();
            ImGui::PopStyleColor();
            
            ImGui::Dummy(ImVec2(0.0f, 2.0f));

            ImGui::Indent(8.0f);
            drawShaderLinkMenu(menu, choices, inspectorEngPtr, styles);
            ImGui::Unindent(8.0f);

            ImGui::Dummy(ImVec2(0, 4.0f));
            ImGui::TreePop(); 
        }
        ImGui::PopStyleColor(3);
    }
    
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    ImGui::PopID();
}