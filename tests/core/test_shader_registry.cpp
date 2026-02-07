#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <string>

#include "core/ShaderRegistry.hpp"
#include "core/logging/Logger.hpp"

TEST_CASE("ShaderRegistry: initialize(false) produces empty registry", "[shader][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ShaderRegistry reg;

    // Factory won't be used because we disable defaults + don't register programs here
    reg.setFactory([](const char*, const char*, const char*, Logger*) -> ShaderProgram* {
        return nullptr;
    });

    REQUIRE(reg.initialize(&logger, false) == true);
    REQUIRE(reg.getNumberOfPrograms() == 0);
    REQUIRE(reg.getProgram("anything") == nullptr);
}

TEST_CASE("ShaderRegistry: registerProgram rejects empty name", "[shader][registry]") {
    Logger logger;
    REQUIRE(logger.initialize());

    ShaderRegistry reg;

    // Even if called, it returns nullptr; we only care about empty-name rejection
    reg.setFactory([](const char*, const char*, const char*, Logger*) -> ShaderProgram* {
        return nullptr;
    });

    REQUIRE(reg.initialize(&logger, false) == true);
    REQUIRE_FALSE(reg.registerProgram("v.vert", "f.frag", ""));
}

// TEST_CASE("ShaderRegistry: duplicate program name is rejected", "[shader][registry]") {
//     Logger logger;
//     REQUIRE(logger.initialize());

//     ShaderRegistry reg;

//     // We must return a non-null pointer for successful registration.
//     // If ShaderProgram ctor touches GL, you cannot construct it here.
//     //
//     // So for now, we SKIP unless you implement a safe test-program factory.
//     //
//     // Best: create IShaderProgram or allow factory to produce a lightweight stub.
//     reg.setFactory([](const char*, const char*, const char*, Logger*) -> ShaderProgram* {
//         return nullptr;
//     });

//     REQUIRE(reg.initialize(&logger, false) == true);

//     // Can't test successful insertion unless factory can create a safe program.
//     SKIP("Need a non-GL ShaderProgram stub to test insertion/duplicate; add initialize(false) already works. Next step: make ShaderProgram mockable or provide safe stub.");
// }
