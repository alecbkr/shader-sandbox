#pragma once

#include <variant>
#include <string>
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
    CreateModel,
    DeleteModel,
    ModelMaterialChange,
    MaterialTypeChange,
    CloneFile
};

struct SaveActiveShaderFilePayload { std::string filePath; unsigned int modelID; };
struct ReloadShaderPayload { std::string programName; };
struct WindowResizePayload { int w, h; };
struct KeyPressedPayload { int key; };
struct OpenFilePayload { std::string filePath; std::string fileName; unsigned int modelID; bool readOnly; };
struct RenameFilePayload { std::string oldName, newName; };
struct DeleteFilePayload { std::string fileName; };
struct CloneFilePayload {std::string fileName; };
struct ModelCreationPayload { unsigned int modelID; };
struct ModelDeletionPayload { unsigned int modelID; };
struct ModelMaterialChangePayload { unsigned int modelID; unsigned int meshIdx; unsigned int materialID; };
struct MaterialTypeChangePayload { unsigned int materialID; MaterialType newType; };

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
    ModelCreationPayload,
    ModelDeletionPayload,
    ModelMaterialChangePayload,
    MaterialTypeChangePayload
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

inline Event OpenFileEvent(std::string filePath, std::string fileName, unsigned int modelID, bool readOnly) {
    return { EventType::OpenFile, false, OpenFilePayload{filePath, fileName, modelID, readOnly} };
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