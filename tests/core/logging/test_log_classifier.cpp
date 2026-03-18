#define CATCH_CONFIG_MAIN 
#include <catch2/catch_amalgamated.hpp>
#include "core/logging/LogClassifier.hpp"

// Note I did use ai for this but I went through the test cases and modified them accordingly 

TEST_CASE("LogClassifier: Coverage & Priorities", "[LogClassifier]") {

    SECTION("Feature: Textures & Assets (Previously Missing)") {
        // These used to be OTHER, now they should be ASSETS
        CHECK(LogClassifier::categorizeByString("./texture/Texture2D.cpp") == LogCategory::ASSETS);
        CHECK(LogClassifier::categorizeByString("./texture/CubeMap.hpp") == LogCategory::ASSETS);
        CHECK(LogClassifier::categorizeByString("./core/TextureRegistry.cpp") == LogCategory::ASSETS);
        
        // Existing object files
        CHECK(LogClassifier::categorizeByString("./object/Model.cpp") == LogCategory::ASSETS);
        CHECK(LogClassifier::categorizeByString("./object/MaterialCache.cpp") == LogCategory::ASSETS);
    }

    SECTION("Feature: Platform & Persistence (Previously Missing)") {
        // These used to be OTHER, now they should be SYSTEM
        CHECK(LogClassifier::categorizeByString("./persistence/ProjectLoader.cpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./persistence/SettingsLoader.hpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./platform/Platform.cpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./platform/components/Window.cpp") == LogCategory::SYSTEM);
    }

    SECTION("Conflict Resolution: UI vs Others") {
        // CRITICAL: This file contains "Uniform" (Shader) and "UI". 
        CHECK(LogClassifier::categorizeByString("./core/ui/UniformInspectorUI.cpp") == LogCategory::UI);
        CHECK(LogClassifier::categorizeByString("./core/ui/ObjectsInspectorUI.cpp") == LogCategory::UI);
        CHECK(LogClassifier::categorizeByString("./core/ui/FileInspectorUI.cpp") == LogCategory::UI);
    }

    SECTION("System Core Components") {
        CHECK(LogClassifier::categorizeByString("./application/Application.cpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./core/input/InputState.cpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./engine/AppTimer.cpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./core/logging/Logger.cpp") == LogCategory::SYSTEM);
        CHECK(LogClassifier::categorizeByString("./core/ConsoleEngine.cpp") == LogCategory::SYSTEM);
    }

    SECTION("Robustness: Case Insensitivity") {
        // The file on disk is "ShaderRegistry", but even if we pass "shaderregistry", 
        // it should still match the "shader" rule.
        CHECK(LogClassifier::categorizeByString("core/shaderregistry.cpp") == LogCategory::SHADER);
        
        // "UI" vs "ui"
        CHECK(LogClassifier::categorizeByString("core/UI/MenuUI.cpp") == LogCategory::UI);
    }
}