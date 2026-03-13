#include "core/ui/Fonts.hpp"

bool Fonts::initialize(u8 index) {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    for (std::size_t i = 0; i < NUMBER_OF_FONT_SIZES + NUMBER_OF_FONT_LEVELS - 1; i++) {
        fonts[i] = io.Fonts->AddFontFromFileTTF(
            "../assets/fonts/Roboto-VariableFont_wdth,wght.ttf",
            (STARTING_FONT_SIZE + (INCREMENT_BETWEEN_LEVELS * i))
        );
    }
    setFontIndex(index);
    io.FontDefault = fonts[L1_INDEX];
    return true;
}

void Fonts::setFontIndex(u8 index) {
    if (index < NUMBER_OF_FONT_SIZES) fontIdx = index;
}

u8 Fonts::getFontIndex() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fontIdx;
}

void Fonts::increaseFont() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    if (fontIdx < (NUMBER_OF_FONT_SIZES - 1)) fontIdx++;
}

void Fonts::decreaseFont() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    if (fontIdx != 0) fontIdx--;
}

ImFont* Fonts::getL1() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L1_INDEX];
}

ImFont* Fonts::getL2() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L2_INDEX];
}

ImFont* Fonts::getL3() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L3_INDEX];
}

ImFont* Fonts::getL4() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L4_INDEX];
}

ImFont* Fonts::getL5() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L5_INDEX];
}

ImFont* Fonts::getL6() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L6_INDEX];
}

ImFont* Fonts::getL7() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L7_INDEX];
}

ImFont* Fonts::getL8() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L8_INDEX];
}

ImFont* Fonts::getL9() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L9_INDEX];
}

ImFont* Fonts::getL10() {
    if (fontIdx >= NUMBER_OF_FONT_SIZES) fontIdx = STARTING_FONT_INDEX;
    return fonts[L10_INDEX];
}