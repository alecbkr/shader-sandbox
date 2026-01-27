#include "engine/AppTimer.hpp"

double AppTimer::currTime = 0.0;
double AppTimer::lastFrameTime = 0.0;
double AppTimer::deltaTime = 0.0;
double AppTimer::elapsedTime = 0.0;
int AppTimer::frameCount = 0;
float AppTimer::fps = 0.0f;

bool AppTimer::initialize() {
    AppTimer::currTime = 0.0;
    AppTimer::lastFrameTime = 0.0;
    AppTimer::deltaTime = 0.0;
    AppTimer::elapsedTime = 0.0;
    AppTimer::frameCount = 0;
    AppTimer::fps = 0.0f;
    return true;
}