#pragma once

#include <imgui/imgui.h>
#include <array>
#include <types.hpp>

#define NUMBER_OF_FONT_SIZES 10
#define STARTING_FONT_INDEX 4
#define NUMBER_OF_FONT_LEVELS 10
#define INCREMENT_BETWEEN_LEVELS 2.5f
#define STARTING_FONT_SIZE 12.0f

#define L1_INDEX fontIdx
#define L2_INDEX fontIdx + 1
#define L3_INDEX fontIdx + 2
#define L4_INDEX fontIdx + 3
#define L5_INDEX fontIdx + 4
#define L6_INDEX fontIdx + 5
#define L7_INDEX fontIdx + 6
#define L8_INDEX fontIdx + 7
#define L9_INDEX fontIdx + 8
#define L10_INDEX fontIdx + 9

class Fonts {
public:
    std::array<ImFont*, NUMBER_OF_FONT_SIZES + NUMBER_OF_FONT_LEVELS - 1> fonts;
    
    bool initialize(u8 index);
    void setFontIndex(u8 index);
    u8 getFontIndex();
    void increaseFont();
    void decreaseFont();
    ImFont* getL1();
    ImFont* getL2();
    ImFont* getL3();
    ImFont* getL4();
    ImFont* getL5();
    ImFont* getL6();
    ImFont* getL7();
    ImFont* getL8();
    ImFont* getL9();
    ImFont* getL10();

    std::array<ImFont*, NUMBER_OF_FONT_SIZES + NUMBER_OF_FONT_LEVELS - 1> monoFonts;
    ImFont* getMonoFont();
private:
    std::size_t fontIdx = STARTING_FONT_INDEX;
};