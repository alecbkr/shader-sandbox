#pragma once
#include "IModal.hpp"
#include "application/AppSettings.hpp"
#include "application/Project.hpp"

class ModelCache;
class MaterialCache;

class OpenProjectModal final : public IModal {
public:
    OpenProjectModal() = default;

    bool initialize(Project* project, AppSettings* settings, ModelCache* _modelCachePtr, MaterialCache* _materialCachePtr, bool* projectSwitch);
    static constexpr const char* ID = "Open Project";
    std::string_view id() const override { return ID; }
    void draw() override;

private:
    Project* projectPtr = nullptr;
    AppSettings* settingsPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    MaterialCache* materialCachePtr = nullptr;
    bool* projectSwitchPtr = nullptr;
    bool initialized = false;
};
