#include "core/input/InputState.hpp"

InputState::InputState() {
    initialized = false;
    mouseX = 0.0;
    mouseY = 0.0;
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
    scrollX = 0.0;
    scrollY = 0.0;
    pressedKeys.clear();
    loggerPtr = nullptr;
}

bool InputState::initialize(Logger* _loggerPtr) {
    if (initialized) {
        loggerPtr->addLog(LogLevel::WARNING, "Input State Initialization", "Input State was already initialized.");
        return false;
    }
    loggerPtr = _loggerPtr;

    mouseX = 0.0;
    mouseY = 0.0;
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
    scrollX = 0.0;
    scrollY = 0.0;
    pressedKeys.clear();
    
    beginFrame();
    //platformPtr->initializeInputCallbacks();

    initialized = true;
    return true;
}

void InputState::shutdown() {
    loggerPtr = nullptr;
    pressedKeys.clear();
}

void InputState::beginFrame() { // call before glfwPollEvents()
    pressed.fill(0);
    released.fill(0);
    mousePressed.fill(0);
    mouseReleased.fill(0);

    pressedKeys.clear();
    
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;

    scrollX = 0.0;
    scrollY = 0.0;
}

bool InputState::isDownKey(Key key) {
    return key != Key::Unknown && down[keyIndex(key)];
}

bool InputState::wasPressed(Key key) {
    return key != Key::Unknown && pressed[keyIndex(key)];
}

bool InputState::wasReleased(Key key) {
    return key != Key::Unknown && released[keyIndex(key)];
}

bool InputState::isDownMouse(MouseButton button) {
    return button != MouseButton::Unknown && mouseDown[buttonIndex(button)];
}

bool InputState::wasPressedMouse(MouseButton button) {
    return button != MouseButton::Unknown && mousePressed[buttonIndex(button)];
}

bool InputState::wasReleasedMouse(MouseButton button) {
    return button != MouseButton::Unknown && mouseReleased[buttonIndex(button)];
}

void InputState::onKey(int _key, int action) {
    Key key = translateGlfwKey(_key);
    if (key == Key::Unknown) return;

    if (action == 1) { // GLFW_PRESS
        if (!down[keyIndex(key)]) {
            pressed[keyIndex(key)] = 1;
            pressedKeys.push_back(key);
        }
        down[keyIndex(key)] = 1;
    } else if (action == 0) { // GLFW_RELEASE
        if (down[keyIndex(key)]) released[keyIndex(key)] = 1;
        down[keyIndex(key)] = 0;
    } else if (action == 2) { // GLFW_REPEAT
        down[keyIndex(key)] = 1;
    }
}

void InputState::onMouseButton(int _button, int action) {
    MouseButton button = translateGlfwMouseButton(_button);
    if (button == MouseButton::Unknown) return;

    if (action == 1) { // GLFW_PRESS
        if (!mouseDown[buttonIndex(button)]) mousePressed[buttonIndex(button)] = 1;
        mouseDown[buttonIndex(button)] = 1;
    } else if (action == 0) { // GLFW_RELEASE
        if (mouseDown[buttonIndex(button)]) mouseReleased[buttonIndex(button)] = 1;
        mouseDown[buttonIndex(button)] = 0;
    }
}

void InputState::onCursorPos(double x, double y) {
    mouseDeltaX += (x - mouseX);
    mouseDeltaY += (y - mouseY);
    mouseX = x;
    mouseY = y;
}

void InputState::onScroll(double xoff, double yoff) {
    scrollX += xoff;
    scrollY += yoff;
}