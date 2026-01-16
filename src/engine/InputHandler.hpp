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

// #include <glfw/glfw3.h>

// #define KEYBOARD    InputHandler::keys
// #define MOUSEBUTTON InputHandler::buttons
// #define CURSOR      InputHandler::cursor
// #define SCROLLWHEEL InputHandler::scroll
// #define WINDOWSIZE  InputHandler::windowSize


// class InputHandler {
// private:
//     static bool initialized;
//     struct PushState {
//         bool isDown     = false;
//         bool isPressed  = false;
//         bool isReleased = false;
//     };

//     struct CursorState {
//         bool firstInput = true;
//         float currPosX  = 0.0f;
//         float currPosY  = 0.0f;
//         float lastPosX  = 0.0f;
//         float lastPosY  = 0.0f;
//         float offsetX   = 0.0f;
//         float offsetY   = 0.0f;
//     };

//     struct ScrollState {
//         float offsetX = 0.0f;
//         float offsetY = 0.0f;
//     };

//     struct WindowState {
//         int width = 1000;
//         int height = 800;
//     };

// public:
//     static PushState keys[GLFW_KEY_LAST];
//     static PushState buttons[GLFW_MOUSE_BUTTON_LAST];
//     static CursorState cursor;
//     static ScrollState scroll;
//     static WindowState windowSize;

//     static bool initialize();
//     static void updateCursor();
//     static void resetStates();
//     static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
//     static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
//     static void cursor_callback(GLFWwindow *window, double xpos, double ypos); 
//     static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
//     static void windowSize_callback(GLFWwindow *window, int widht, int height);
// };

#endif