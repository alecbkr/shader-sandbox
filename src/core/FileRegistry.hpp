#pragma once
#include <string>
#include <unordered_map>

#include "EventTypes.hpp"

enum FileState {
    NONE,
    RENAME,
    DELETE,
    NEW
};

struct ShaderFile {
    ShaderFile(std::string filePath, std::string fileName);
    FileState state;
    std::string filePath;
    std::string fileName;
    std::string renameBuffer;
};

class FileRegistry {
public:
    static bool initialize();
    static void reloadMap();
    static std::unordered_map<std::string, ShaderFile *> getFiles();

private:
    static std::unordered_map<std::string, ShaderFile*> files;
    static bool initialized;
    static bool renameFile(const EventPayload& payload);
    static bool deleteFile(const EventPayload& payload);
};

