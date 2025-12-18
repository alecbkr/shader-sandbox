// DESCRIPTION
/*
InputHandler is a single instance of input states for ease of access when assigning
keybinds or controls.

KEYBOARD    - accesses the array of key states for entire keyboard.
              records isPressed, isDown, isReleased.

              To check a key's state do: KEYBOARD[GLFW_KEY].isState
              For example KEYBOARD[GLFW_KEY_W].isDown, returns true or false


MOUSEBUTTON - accesses the array of button states for a mouse.
              exact same functionality as KEYBOARD, but with mouse buttons


CURSOR      - records mouse location, previous location, and offset.


SCROLLWHEEL - records xoffset and yoffset


INPUT       - the raw reference to the instance, only needed when calling resetStates()


resetStates() - call at the end of a frame to ensure no temporary 
                states (isPressed/isReleased) linger.
*/

#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <glfw/glfw3.h>
#include <glm/vec2.hpp>

#define INPUT InputHandler::getInstance()
#define KEYBOARD InputHandler::getInstance().keys
#define MOUSEBUTTON InputHandler::getInstance().buttons
#define CURSOR InputHandler::getInstance().cursor
#define SCROLLWHEEL InputHandler::getInstance().scroll


struct PushState {
    bool isDown = false;
    bool isPressed = false;
    bool isReleased = false;
};

struct CursorState {
    bool firstInput = true;
    float currPosX = 0.0f;
    float currPosY = 0.0f;
    float lastPosX = 0.0f;
    float lastPosY = 0.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
};

struct ScrollState {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
};

class InputHandler {
    public:
        PushState keys[GLFW_KEY_LAST];
        PushState buttons[GLFW_MOUSE_BUTTON_LAST];
        CursorState cursor;
        ScrollState scroll;

        static InputHandler &getInstance();
        void resetStates();
        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
        static void cursor_callback(GLFWwindow *window, double xpos, double ypos); 
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
        
        private:
            InputHandler();
};

#endif