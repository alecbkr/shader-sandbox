#define CATCH_CONFIG_MAIN
#include <catch2/catch_amalgamated.hpp>

#include "imgui.h"
#include "core/ui/components/TextSelector.hpp"

#include <vector>
#include <string>

// Global string to hold the output during testing
static std::string g_TestHookOutput;

// ========================================================================
// 1. FRIEND BRIDGE (Access Private Methods)
// ========================================================================
struct TextSelectorTester {
    static void GetWordUnderCursor(const std::string& text, int col, int& outStart, int& outEnd) {
        TextSelector::getWordUnderCursor(text, col, outStart, outEnd);
    }

    static bool IsDelimeter(char c) {
        return TextSelector::isDelimeter(c);
    }

    static bool IsWhiteSpace(char c) {
        return TextSelector::isWhiteSpace(c);
    }

    // Set the hook from the test file
    static void SetClipboardHook(std::function<void(const std::string&)> hook) {
        TextSelector::s_testClipboardHook = hook;
    }
};

// ========================================================================
// 2. IMGUI FIXTURE
// ========================================================================
struct ImGuiFixture {
    ImGuiContext* ctx;

    ImGuiFixture() {
        // 1. Create Context (Required so CalcTextSize doesn't crash)
        ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);

        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
        io.Fonts->AddFontDefault();

        // 2. Wire up our direct C++ hook!
        g_TestHookOutput.clear();
        TextSelectorTester::SetClipboardHook([](const std::string& text) {
            g_TestHookOutput = text;
        });
    }

    ~ImGuiFixture() {
        // Clean up the hook
        TextSelectorTester::SetClipboardHook(nullptr);
        ImGui::DestroyContext(ctx);
    }
};

// ========================================================================
// 3. TEST CASES
// ========================================================================

TEST_CASE_METHOD(ImGuiFixture, "Copy Functionality", "[TextSelector][Copy]") {
    TextSelectionCtx ctx;
    ctx.isActive = true;
    ctx.mode = SelectionMode::Normal;

    std::vector<std::string> lines = { 
        "[INFO: TEXTURE2D] Loading texture...", 
        "[INFO: CUBEMAP] Loading texture..." 
    };
    
    auto fetchLine = [&](int i, bool& wrap) -> std::string { 
        wrap = false; 
        if (i >= 0 && i < lines.size()) return lines[i];
        return ""; 
    };

    SECTION("Single Line Copy") {
        ctx.startRow = 0; ctx.startCol = 0;
        ctx.endRow = 0;   ctx.endCol = 5;

        TextSelector::copyText(ctx, 2, fetchLine);
        
        // Assert against our direct hook output!
        REQUIRE(g_TestHookOutput == "[INFO");
    }

    SECTION("Multi-line Copy") {
        ctx.startRow = 0; ctx.startCol = 0;
        ctx.endRow = 1;   ctx.endCol = 15; 

        TextSelector::copyText(ctx, 2, fetchLine);
        REQUIRE(g_TestHookOutput == "[INFO: TEXTURE2D] Loading texture...\n[INFO: CUBEMAP]");
    }

    SECTION("Backwards Selection Copy") {
        ctx.startRow = 1; ctx.startCol = 15;
        ctx.endRow = 0;   ctx.endCol = 0;

        TextSelector::copyText(ctx, 2, fetchLine);
        REQUIRE(g_TestHookOutput == "[INFO: TEXTURE2D] Loading texture...\n[INFO: CUBEMAP]");
    }
    
    SECTION("Line Mode Copy") {
        ctx.mode = SelectionMode::Line;
        ctx.startRow = 1; 
        ctx.endRow = 1;
        ctx.startCol = 100; 
        ctx.endCol = 0; 

        TextSelector::copyText(ctx, 2, fetchLine);
        REQUIRE(g_TestHookOutput == "[INFO: CUBEMAP] Loading texture...");
    }
}