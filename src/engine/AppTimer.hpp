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

        static AppTimer &getInstance() {
            static AppTimer instance;
            return instance;
        }

        void update() {
            lastFrameTime = currTime;
            currTime = glfwGetTime();
            deltaTime = currTime - lastFrameTime;
        }

        float getDt() {
            return deltaTime;
        }
        
        float getFPS() {
            elapsedTime += deltaTime;
            frameCount++;

            if (elapsedTime >= 2.0f) {
                fps = frameCount / elapsedTime;
                elapsedTime = 0;
                frameCount = 0;
            }
            return fps;
        }


        private:
            
            double currTime;
            double lastFrameTime;
            double deltaTime;
            double elapsedTime;
            int frameCount;
            float fps;
            
            AppTimer() {
                currTime = 0;
                lastFrameTime = 0;
                deltaTime = 0;

                elapsedTime = 0;
                frameCount = 0;
                fps = 0;
            }

};

#endif