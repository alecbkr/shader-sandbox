#pragma once

#include <variant>
#include <string>
#include <vector>
#include "object/MaterialType.hpp"

enum class EventType {
    // Fill out event types here
    NoType,
    WindowResize,
    KeyPressed,
    SaveActiveShaderFile,
    ReloadShader,
    NewProject,
    SaveProject,
    RenameProject,
    Quit,
    OpenFile,
    NewFile,
    RenameFile,
    ET_DeleteFile,
    ContextSwitch,
    CloneFile,
    ToggleEditorFind,
    LoadModel,
    UploadToRenderer,
    DeleteFromRenderer,
    MaterialValidated,
    MaterialsInvalidated,
    MaterialTypeChange,
    ProgramDeleted
};

struct SaveActiveShaderFilePayload { std::string filePath; unsigned int modelID; };
struct ReloadShaderPayload { std::string programName; };
struct WindowResizePayload { int w, h; };
struct KeyPressedPayload { int key; };
struct OpenFilePayload { std::string filePath; std::string fileName; bool readOnly; };
struct RenameFilePayload { std::string oldName, newName; };
struct DeleteFilePayload { std::string fileName; };
struct CloneFilePayload {std::string fileName; };
struct LoadModelPayload { std::string filePath; }; 
struct UploadToRendererPayload { unsigned int modelID; bool isSkybox; };
struct DeleteFromRendererPayload { unsigned int modelID; };
struct MaterialValidatedPayload { unsigned int materialID; };
struct MaterialsInvalidatedPayload { std::vector<unsigned int> invalidMaterialIDs; };
struct MaterialTypeChangePayload { unsigned int materialID; MaterialType newType; };
struct ProgramDeletedPayload { unsigned int programID; };

using EventPayload = std::variant<
    std::monostate,
    SaveActiveShaderFilePayload,
    ReloadShaderPayload,
    WindowResizePayload,
    KeyPressedPayload,
    OpenFilePayload,
    RenameFilePayload,
    DeleteFilePayload,
    CloneFilePayload,
    LoadModelPayload,
    UploadToRendererPayload,
    DeleteFromRendererPayload,
    MaterialValidatedPayload,
    MaterialsInvalidatedPayload,
    MaterialTypeChangePayload,
    ProgramDeletedPayload
>;

struct Event {
    EventType type;
    bool handled = false;
    EventPayload payload;
};

inline Event MakeSaveActiveShaderFileEvent(std::string filePath, unsigned int modelID) {
    return { EventType::SaveActiveShaderFile, false, SaveActiveShaderFilePayload{filePath, modelID} };
};

inline Event MakeReloadShaderEvent(std::string programName) {
    return { EventType::ReloadShader, false, ReloadShaderPayload{programName} };
};

inline Event MakeWindowResizeEvent(int w, int h) {
    return { EventType::WindowResize, false, WindowResizePayload{w, h} };
};

inline Event MakeKeyPressedEvent(int key) {
    return { EventType::KeyPressed, false, KeyPressedPayload{key} };
};

inline Event MakeQuitAppEvent() {
    return { EventType::Quit, false, std::monostate{} };
};

inline Event OpenFileEvent(std::string filePath, std::string fileName, bool readOnly) {
    return { EventType::OpenFile, false, OpenFilePayload{filePath, fileName, readOnly} };
}

inline Event NewFileEvent() {
    return { EventType::NewFile, false, std::monostate{} };
}

inline Event RenameFileEvent(std::string oldName, std::string newName) {
    return { EventType::RenameFile, false, RenameFilePayload{oldName, newName} };
}

inline Event DeleteFileEvent(std::string fileName) {
    return { EventType::ET_DeleteFile, false, DeleteFilePayload{fileName} };
}

inline Event CloneFileEvent(std::string fileName) {
    return { EventType::CloneFile, false, CloneFilePayload{fileName} };
};

inline Event LoadModelEvent(std::string filePath) {
    return { EventType::LoadModel, false, LoadModelPayload{filePath} }; 
}