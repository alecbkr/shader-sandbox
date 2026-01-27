#pragma once

#include <array>
#include <vector>
#include <cstdint>

#include "platform/components/Keys.hpp"

class InputState {
public:
    static constexpr int KEY_COUNT = keyIndex(Key::Count);
    static constexpr int MOUSE_COUNT = buttonIndex(MouseButton::Count);
    static std::vector<Key> pressedKeys;

    static bool initialize();
    static void beginFrame(); // call before glfwPollEvents()
    static bool isDownKey(Key key);
    static bool wasPressed(Key key);
    static bool wasReleased(Key key);
    static bool isDownMouse(MouseButton button);
    static bool wasPressedMouse(MouseButton button);
    static bool wasReleasedMouse(MouseButton button);
    static void onKey(int _key, int action);
    static void onMouseButton(int _button, int action);
    static void onCursorPos(double x, double y);
    static void onScroll(double xoff, double yoff);

private:
    static std::array<uint8_t, KEY_COUNT> down;
    static std::array<uint8_t, KEY_COUNT> pressed;
    static std::array<uint8_t, KEY_COUNT> released;

    static std::array<uint8_t, MOUSE_COUNT> mouseDown;
    static std::array<uint8_t, MOUSE_COUNT> mousePressed;
    static std::array<uint8_t, MOUSE_COUNT> mouseReleased;

    static double mouseX;
    static double mouseY;
    static double mouseDeltaX;
    static double mouseDeltaY;
    
    static double scrollX;
    static double scrollY;
};
