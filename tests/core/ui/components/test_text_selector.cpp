#define CATCH_CONFIG_MAIN
#include <catch2/catch_amalgamated.hpp>

#include "imgui.h"
#include "core/ui/components/TextSelector.hpp"
#include <vector>
#include <string>

// ========================================================================
// 1. GLOBAL TEST HOOK STATE
// ========================================================================
// This string will hold whatever TextSelector tries to copy to the clipboard.
static std::string g_TestHookOutput;

// ========================================================================
// 2. FRIEND BRIDGE
// ========================================================================
// This struct matches the 'friend' declaration in your TextSelector class.
// It allows us to bypass private access modifiers during testing.
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

    // Bridge to set our direct C++ clipboard hook
    static void SetClipboardHook(std::function<void(const std::string&)> hook) {
        TextSelector::s_testClipboardHook = hook;
    }
};

// ========================================================================
// 3. FIXTURE SETUP
// ========================================================================
struct ImGuiFixture {
    ImGuiContext* ctx;

    ImGuiFixture() {
        // 1. Basic ImGui context setup (Required so CalcTextSize doesn't crash)
        ctx = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx);

        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
        io.Fonts->AddFontDefault();

        // 2. Wire up our direct C++ test hook!
        // This completely bypasses ImGui's internal clipboard routing, 
        // avoiding all DLL/static linking boundary issues.
        g_TestHookOutput.clear();
        TextSelectorTester::SetClipboardHook([](const std::string& text) {
            g_TestHookOutput = text;
        });
    }

    ~ImGuiFixture() {
        // Clean up the hook so it doesn't leak into other tests
        TextSelectorTester::SetClipboardHook(nullptr);
        ImGui::DestroyContext(ctx);
    }
};

// ========================================================================
// 4. TEST CASES
// ========================================================================

TEST_CASE_METHOD(ImGuiFixture, "Internal: Word Boundary Detection", "[TextSelector][Internal]") {
    
    SECTION("Basic Word Selection") {
        std::string text = "Hello World";
        int start = 0, end = 0;
        
        // "Hello" is indices 0-5
        TextSelectorTester::GetWordUnderCursor(text, 0, start, end);
        REQUIRE(text.substr(start, end - start) == "Hello");

        // "World" is indices 6-11
        TextSelectorTester::GetWordUnderCursor(text, 8, start, end);
        REQUIRE(text.substr(start, end - start) == "World");
    }

    SECTION("Delimiter Handling") {
        std::string text = "func(arg)";
        int start = 0, end = 0;

        // Cursor at '(' (index 4)
        TextSelectorTester::GetWordUnderCursor(text, 4, start, end);
        REQUIRE(start == 4);
        REQUIRE(end == 5); 
        REQUIRE(text.substr(start, end - start) == "(");
    }

    SECTION("Whitespace Handling") {
        std::string text = "A   B";
        int start = 0, end = 0;

        // Cursor inside spaces
        TextSelectorTester::GetWordUnderCursor(text, 2, start, end);
        REQUIRE(text.substr(start, end - start) == "   ");
    }
}

TEST_CASE_METHOD(ImGuiFixture, "Internal: Character Classification", "[TextSelector][Internal]") {
    SECTION("IsDelimeter") {
        REQUIRE(TextSelectorTester::IsDelimeter('('));
        REQUIRE(TextSelectorTester::IsDelimeter('"'));
        REQUIRE_FALSE(TextSelectorTester::IsDelimeter('A'));
    }

    SECTION("IsWhiteSpace") {
        REQUIRE(TextSelectorTester::IsWhiteSpace(' '));
        REQUIRE(TextSelectorTester::IsWhiteSpace('\t'));
        REQUIRE_FALSE(TextSelectorTester::IsWhiteSpace('A'));
    }
}

TEST_CASE_METHOD(ImGuiFixture, "Copy Functionality", "[TextSelector][Copy]") {
    // 1. Setup the Selection Context
    TextSelectionCtx ctx;
    ctx.isActive = true;
    ctx.mode = SelectionMode::Normal;

    // 2. Setup the Mock Text Provider
    std::vector<std::string> lines = { 
        "[INFO: TEXTURE2D] Loading texture...", 
        "[INFO: CUBEMAP] Loading texture...",
        "[INFO: SHADER] Compiling shader..."
    };
    
    // Total rows is now 3
    int totalRows = (int)lines.size();

    auto fetchLine = [&](int i, bool& wrap) -> std::string { 
        wrap = false; 
        if (i >= 0 && i < totalRows) return lines[i];
        return ""; 
    };

    // 3. The Test Sections

    SECTION("Single Line Copy") {
        // Select "[INFO" (indices 0-5) from line 0
        ctx.startRow = 0; ctx.startCol = 0;
        ctx.endRow = 0;   ctx.endCol = 5;

        TextSelector::copyText(ctx, totalRows, fetchLine);
        
        REQUIRE(g_TestHookOutput == "[INFO");
    }

    SECTION("Multi-line Copy (Top to Bottom)") {
        // Select from start of line 0 up to the 'P' in CUBEMAP (col 15) on line 1
        ctx.startRow = 0; ctx.startCol = 0;
        ctx.endRow = 1;   ctx.endCol = 15; 

        TextSelector::copyText(ctx, totalRows, fetchLine);
        
        REQUIRE(g_TestHookOutput == "[INFO: TEXTURE2D] Loading texture...\n[INFO: CUBEMAP]");
    }

    SECTION("Backwards Selection Copy (Bottom to Top)") {
        // Drag from the 'P' in CUBEMAP (col 15 on line 1) backwards to the start of the first line
        ctx.startRow = 1; ctx.startCol = 15;
        ctx.endRow = 0;   ctx.endCol = 0;

        TextSelector::copyText(ctx, totalRows, fetchLine);
        
        // The copy logic should auto-swap the coordinates and produce the exact same result
        REQUIRE(g_TestHookOutput == "[INFO: TEXTURE2D] Loading texture...\n[INFO: CUBEMAP]");
    }

    SECTION("In-between Multiline Copy") {
        // Start on Line 0, Col 7 (The 'T' in TEXTURE2D)
        ctx.startRow = 0; 
        ctx.startCol = 7; 
        
        // End on Line 2, Col 14 (Right after the ']' in SHADER])
        ctx.endRow = 2;   
        ctx.endCol = 14; 

        TextSelector::copyText(ctx, totalRows, fetchLine);
        
        // Expected Output:
        // 1. End of the first line
        // 2. The ENTIRE second line (handled by the 'else' block in copyText)
        // 3. The beginning of the third line
        std::string expectedOutput = 
            "TEXTURE2D] Loading texture...\n"
            "[INFO: CUBEMAP] Loading texture...\n"
            "[INFO: SHADER]";

        REQUIRE(g_TestHookOutput == expectedOutput);
    }
    
    SECTION("Line Mode Copy") {
        ctx.mode = SelectionMode::Line;
        
        // User triple-clicked on the CUBEMAP line (Row 1)
        ctx.startRow = 1; 
        ctx.endRow = 1;
        
        // Cols don't matter in Line Mode, but the UI usually sets them to extremes
        ctx.startCol = 100; 
        ctx.endCol = 0; 

        TextSelector::copyText(ctx, totalRows, fetchLine);
        
        // Line mode should grab the entire row 1 regardless of column indices
        REQUIRE(g_TestHookOutput == "[INFO: CUBEMAP] Loading texture...");
    }
}