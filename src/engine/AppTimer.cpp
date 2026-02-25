#include "engine/AppTimer.hpp"
#include "core/logging/Logger.hpp"
#include "platform/Platform.hpp"

AppTimer::AppTimer() {
    initialized = false;
    currTime = 0.0;
    lastFrameTime = 0.0;
    deltaTime = 0.0;
    elapsedTime = 0.0;
    frameCount = 0;
    fps = 0.0f;
    loggerPtr = nullptr;
    platformPtr = nullptr;
}

bool AppTimer::initialize(Logger* _loggerPtr, Platform* _platformPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "App Timer Initialize", "App Timer has already been initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    platformPtr = _platformPtr;
    nowFn = [this]() { return platformPtr->getTime(); };
    initialized = true;
    return true;
}

bool AppTimer::initialize(Logger* _loggerPtr, std::function<double()> timeFn) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "App Timer Initialize", "App Timer has already been initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;
    platformPtr = nullptr;
    nowFn = std::move(timeFn);

    initialized = true;
    return true;
}

void AppTimer::update() {
    if (!initialized) return;
    lastFrameTime = currTime;
    currTime = nowFn ? nowFn() : 0.0;
    deltaTime = currTime - lastFrameTime;
}

float AppTimer::getDt() {
    return deltaTime;
}

float AppTimer::getFPS() {
    elapsedTime += deltaTime;
    frameCount++;

    if (elapsedTime >= 1.0f) {
        fps = frameCount / elapsedTime;
        elapsedTime = 0;
        frameCount = 0;
    }
    return fps;
}