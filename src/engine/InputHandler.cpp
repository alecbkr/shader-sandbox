#include "InputHandler.hpp"

#include "Errorlog.hpp"


InputHandler& InputHandler::getInstance() {
    static InputHandler instance;
    return instance;
}


void InputHandler::updateCursor() {
    CURSOR.offsetX = CURSOR.currPosX - CURSOR.lastPosX;
    CURSOR.offsetY = CURSOR.lastPosY - CURSOR.currPosY; //swapped on purpose
    CURSOR.lastPosX = CURSOR.currPosX;
    CURSOR.lastPosY = CURSOR.currPosY;
}


void InputHandler::resetStates() {
    for (int i=0; i<GLFW_MOUSE_BUTTON_LAST; i++) {
        buttons[i].isPressed = false;
        buttons[i].isReleased = false;
        keys[i].isPressed = false;
        keys[i].isReleased = false;
    }
    for (int i=GLFW_MOUSE_BUTTON_LAST; i<GLFW_KEY_LAST; i++) {
        keys[i].isPressed = false;
        keys[i].isReleased = false;
    }
    cursor.offsetX = 0.0f;
    cursor.offsetY = 0.0f;
    scroll.offsetX = 0.0f;
    scroll.offsetY = 0.0f;
}


void InputHandler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key < 0) {
        ERRLOG.logEntry(EL_WARNING, "INPUT", "Key is undefined (-1)");
        return;
    }

    PushState &k = INPUT.keys[key];
    if (action == GLFW_PRESS) {
        if (k.isDown == false) {
            k.isPressed = true;
            // ERRLOG.logEntry(EL_INFO, "Input", "Key is PRESSED: ", key);
        }
        k.isDown = true;
        // ERRLOG.logEntry(EL_INFO, "Input", "Key is DOWN: ", key);
    }
    else if (action == GLFW_RELEASE) {
        k.isDown = false;
        k.isReleased = true;
        // ERRLOG.logEntry(EL_INFO, "Input", "Key is RELEASED: ", key);
    }
}


void InputHandler::mouse_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button < 0) {
        ERRLOG.logEntry(EL_WARNING, "INPUT", "Button is undefined (-1)");
        return;
    }

    PushState &b = INPUT.buttons[button];
    if (action == GLFW_PRESS) {
        if (b.isDown == false) {
            b.isPressed = true;
            // ERRLOG.logEntry(EL_INFO, "Input", "Key is PRESSED: ", button);
        }
        b.isDown = true;
        // ERRLOG.logEntry(EL_INFO, "Input", "Key is DOWN: ", button);
    }
    else if (action == GLFW_RELEASE) {
        b.isDown = false;
        b.isReleased = true;
        // ERRLOG.logEntry(EL_INFO, "Input", "Key is RELEASED: ", button);
    }
}


void InputHandler::cursor_callback(GLFWwindow *window, double xpos, double ypos) {
    if (CURSOR.firstInput == true) {
        CURSOR.lastPosX = xpos;
        CURSOR.lastPosY = ypos;
        CURSOR.firstInput = false;
    }
    CURSOR.currPosX = static_cast<float>(xpos);
    CURSOR.currPosY = static_cast<float>(ypos);
}


void InputHandler::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    SCROLLWHEEL.offsetX += static_cast<float>(xoffset);
    SCROLLWHEEL.offsetY += static_cast<float>(yoffset);
}


InputHandler::InputHandler() {

}
