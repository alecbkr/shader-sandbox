#pragma once

#include <array>
#include <vector>
#include <cstdint>

#include "platform/components/Keys.hpp"
#include "core/logging/Logger.hpp"

class InputState {
public:
    static constexpr std::size_t KEY_COUNT = static_cast<std::size_t>(Key::Count);
    static constexpr std::size_t MOUSE_COUNT = static_cast<std::size_t>(MouseButton::Count);
    std::vector<Key> pressedKeys;

    InputState();
    bool initialize(Logger* _loggerPtr);
    void shutdown();
    void beginFrame(); // call before glfwPollEvents()
    bool isDownKey(Key key);
    bool wasPressed(Key key);
    bool wasReleased(Key key);
    bool isDownMouse(MouseButton button);
    bool wasPressedMouse(MouseButton button);
    bool wasReleasedMouse(MouseButton button);
    void onKey(int _key, int action);
    void onMouseButton(int _button, int action);
    void onCursorPos(double x, double y);
    void onScroll(double xoff, double yoff);
    double getMouseDeltaX() const;
    double getMouseDeltaY() const;
    double getScrollX() const;
    double getScrollY() const;

private:
    bool initialized = false;
    Logger* loggerPtr = nullptr;
    std::array<uint8_t, KEY_COUNT> down;
    std::array<uint8_t, KEY_COUNT> pressed;
    std::array<uint8_t, KEY_COUNT> released;

    std::array<uint8_t, MOUSE_COUNT> mouseDown;
    std::array<uint8_t, MOUSE_COUNT> mousePressed;
    std::array<uint8_t, MOUSE_COUNT> mouseReleased;

    double mouseX = 0.0;
    double mouseY = 0.0;
    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;
    
    double scrollX = 0.0;
    double scrollY = 0.0;
};
