// DESCRIPTION
/*
AppTimer is a single instance application time tracker. Access time functions
with the prefix APPTIME.


*/

#ifndef APPTIMER_HPP
#define APPTIMER_HPP

#include <glfw/glfw3.h>

#define APPTIME AppTimer::getInstance()

class AppTimer {
public:

    static bool initialize();

    static void update() {
        lastFrameTime = currTime;
        currTime = glfwGetTime();
        deltaTime = currTime - lastFrameTime;
    }

    static float getDt() {
        return deltaTime;
    }

    static float getFPS() {
        elapsedTime += deltaTime;
        frameCount++;

        if (elapsedTime >= 1.0f) {
            fps = frameCount / elapsedTime;
            elapsedTime = 0;
            frameCount = 0;
        }
        return fps;
    }

private:
    static double currTime;
    static double lastFrameTime;
    static double deltaTime;
    static double elapsedTime;
    static int frameCount;
    static float fps;
};

#endif