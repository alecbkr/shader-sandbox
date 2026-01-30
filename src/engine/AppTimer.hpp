// DESCRIPTION
/*
AppTimer is a single instance application time tracker. Access time functions
with the prefix APPTIME.


*/
#pragma once

#include "core/logging/Logger.hpp"
#include "platform/Platform.hpp"

#define APPTIME AppTimer::getInstance()

class AppTimer {
public:
    AppTimer();
    bool initialize(Logger* _loggerPtr, Platform* _platformPtr);
    void update();
    float getDt();
    float getFPS();

private:
    bool initialized = false;
    double currTime = 0.0;
    double lastFrameTime = 0.0;
    double deltaTime = 0.0;
    double elapsedTime = 0.0;
    int frameCount = 0;
    float fps = 0.0f;
    Logger* loggerPtr;
    Platform* platformPtr;
};