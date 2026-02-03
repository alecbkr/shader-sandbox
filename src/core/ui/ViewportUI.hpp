#ifndef VIEWPORTUI_HPP
#define VIEWPORTUI_HPP

#include "platform/GL.hpp"
#include <memory>
#include <imgui/imgui.h>
#include "engine/Camera.hpp"

class Logger;
class Platform;
class ModelCache;
class AppTimer;

class ViewportUI {
public:
    ViewportUI();
    bool initialize(Logger* _loggerPtr, Platform* _platformPtr, ModelCache* _modelCachePtr, AppTimer* _timerPtr);
    void render();
    Camera* getCamera();
    ~ViewportUI();
    
    private:
    bool initialized = false;
    bool initPos = true;
    GLuint fbo = 0, rbo = 0, viewportTex = 0;
    ImVec2 dimensions = ImVec2(0, 0);
    ImVec2 prevDimensions = ImVec2(0, 0);
    ImVec2 pos = ImVec2(0, 0);
    std::unique_ptr<Camera> camPtr = nullptr;

    float targetWidth = 0.0f;
    float targetHeight = 0.0f;
    ImVec2 windowPos = ImVec2(0, 0);

    Logger* loggerPtr = nullptr;
    Platform* platformPtr = nullptr;
    ModelCache* modelCachePtr = nullptr;
    AppTimer* timerPtr = nullptr;

    void bind();
    void unbind();
    void draw();
    void reformat();
    float getAspect();
};

#endif