#pragma once
#include "IModal.hpp"
#include "application/Project.hpp"
#include "persistence/ProjectSwitch.h"

class ModelCache;
class MaterialCache;
class ShaderRegistry;

class DeleteProjectModal final : public IModal {
public:
    DeleteProjectModal() = default;

    bool initialize(Project* project, ProjectSwitch* projectSwitch, Logger* _loggerPtr);
    static constexpr const char* ID = "Delete Project";
    std::string_view id() const override { return ID; }
    void draw() override;

private:
    Project* projectPtr = nullptr;
    ProjectSwitch* projectSwitchPtr = nullptr;
    Logger* loggerPtr = nullptr;
    bool initialized = false;
    std::string pendingDeletion = "";
};
