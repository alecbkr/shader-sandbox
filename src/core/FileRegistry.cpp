#include "core/FileRegistry.hpp"

#include <filesystem>
#include <iostream>

#include "EventDispatcher.hpp"
#include "logging/Logger.hpp"

std::unordered_map<std::string, ShaderFile*> FileRegistry::files;
bool FileRegistry::initialized = false;

ShaderFile::ShaderFile(std::string filePath, std::string fileName, std::string extension) {
    this->state = NONE;
    this->filePath = filePath;
    this->fileName = fileName;
    this->renameBuffer = fileName;
    this->extension = extension;
}

bool FileRegistry::initialize() {
    EventDispatcher::Subscribe(RenameFile, renameFile);
    EventDispatcher::Subscribe(DeleteFile, deleteFile);
    reloadMap();

    initialized = true;
    return true;
}

void FileRegistry::reloadMap() {
    std::unordered_map<std::string, ShaderFile*> tempMap;
    for (const auto & dirEntry : std::filesystem::directory_iterator("../shaders/")) {
        std::string filePath = dirEntry.path().string();
        std::string fileName = dirEntry.path().filename().string();
        std::string fileExtension = dirEntry.path().extension().string();
        if (!tempMap.contains(fileName)) {
            ShaderFile* shaderFile = new ShaderFile(filePath, fileName, fileExtension);

            if (files.contains(fileName)) {
                shaderFile->state = files.at(fileName)->state;
                shaderFile->renameBuffer = files.at(fileName)->renameBuffer;
            }

            tempMap.emplace(fileName, shaderFile);
        }
    }

    files.swap(tempMap);

    for (auto & file : tempMap) delete file.second;
    tempMap.clear();
}

bool FileRegistry::renameFile(const EventPayload& payload) {
    if (const auto* data = std::get_if<RenameFilePayload>(&payload)) {
        if (files.contains(data->oldName)) {
            ShaderFile* shaderFile = files.at(data->oldName);
            try {
                std::filesystem::path newPath = shaderFile->filePath;
                newPath.replace_filename(data->newName);
                std::filesystem::rename(shaderFile->filePath, newPath);

                shaderFile->fileName = data->newName;
                shaderFile->filePath = newPath.string();
                shaderFile->state = NONE;

                files.emplace(data->newName, shaderFile);
                files.erase(data->oldName);
            } catch (const std::filesystem::filesystem_error& e) {
                Logger::addLog(LogLevel::ERROR, "FileRegistry::renameFile", std::string("Filesystem error: ") + e.what());
            }
        } else {
            Logger::addLog(LogLevel::ERROR, "FileRegistry::renameFile", "File being renamed does not exist.");
        }
    } else {
        Logger::addLog(LogLevel::ERROR, "FileRegistry::renameFile", "Invalid Payload Type");
    }

    return false;
}

bool FileRegistry::deleteFile(const EventPayload& payload) {
    if (const auto* data = std::get_if<DeleteFilePayload>(&payload)) {
        if (files.contains(data->fileName)) {
            ShaderFile* shaderFile = files.at(data->fileName);
            try {
                if (std::filesystem::exists(shaderFile->filePath)) {
                    std::filesystem::remove(shaderFile->filePath);
                }

                delete shaderFile;
                files.erase(data->fileName);
            } catch (const std::filesystem::filesystem_error& e) {
                Logger::addLog(LogLevel::ERROR, "FileRegistry::deleteFile", std::string("Filesystem error: ") + e.what());
            }
        } else {
            Logger::addLog(LogLevel::ERROR, "FileRegistry::deleteFile", "File being deleted does not exist.");
        }
    }

    return false;
}

std::unordered_map<std::string, ShaderFile *> FileRegistry::getFiles() {
    return files;
}
