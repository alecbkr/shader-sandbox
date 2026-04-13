#include <catch2/catch_amalgamated.hpp>

#include <unordered_map>

#include "application/Project.hpp"
#include "core/UniformRegistry.hpp"
#include "core/UniformTypes.hpp"
#include "core/logging/Logger.hpp"

namespace {

static bool initTestLogger(Logger& logger) {
    return logger.initialize("ShaderSandbox_Test", "UniformRegistry_Tests");
}

static Uniform makeFloatUniform(const std::string& name, float value = 0.f) {
    Uniform u;
    u.name = name;
    u.type = UniformType::Float;
    u.value = value;
    u.ID = 0;
    u.materialID = 0;
    return u;
}

} // namespace

TEST_CASE("UniformRegistry: initialize twice returns false", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project) == true);
    REQUIRE(reg.initialize(&logger, &project) == false);
}

TEST_CASE("UniformRegistry: re-registering material uniform preserves ID", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    Uniform first = makeFloatUniform("uTime", 1.f);
    reg.registerMaterialUniform(7, first);

    const Uniform* readBack = reg.tryReadMaterialUniform(7, "uTime");
    REQUIRE(readBack != nullptr);
    const unsigned int id = readBack->ID;

    Uniform second = makeFloatUniform("uTime", 42.f);
    reg.registerMaterialUniform(7, second);

    REQUIRE(reg.tryReadMaterialUniform(7, "uTime")->ID == id);
    REQUIRE(project.uniforms.count(id) == 1);
    REQUIRE(std::get<float>(project.uniforms.at(id).value) == Catch::Approx(42.f));
}

TEST_CASE("UniformRegistry: registerMaterialUniformMap reuses IDs for existing names", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    Uniform a = makeFloatUniform("alpha", 0.5f);
    reg.registerMaterialUniform(3, a);
    const unsigned int alphaId = reg.tryReadMaterialUniform(3, "alpha")->ID;

    std::unordered_map<std::string, Uniform> map;
    Uniform alphaNew = makeFloatUniform("alpha", 99.f);
    Uniform beta = makeFloatUniform("beta", 2.f);
    map["alpha"] = alphaNew;
    map["beta"] = beta;

    reg.registerMaterialUniformMap(3, map);

    REQUIRE(reg.tryReadMaterialUniform(3, "alpha")->ID == alphaId);
    REQUIRE(reg.containsMaterialUniform(3, "beta"));
    REQUIRE(reg.tryReadMaterialUniform(3, "beta")->ID != alphaId);

    REQUIRE(project.uniforms.count(alphaId) == 1);
    REQUIRE(std::get<float>(project.uniforms.at(alphaId).value) == Catch::Approx(99.f));
}

TEST_CASE("UniformRegistry: eraseMaterial removes uniforms from project storage", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    reg.registerMaterialUniform(10, makeFloatUniform("x", 1.f));
    reg.registerMaterialUniform(10, makeFloatUniform("y", 2.f));

    const unsigned int idX = reg.tryReadMaterialUniform(10, "x")->ID;
    const unsigned int idY = reg.tryReadMaterialUniform(10, "y")->ID;

    reg.eraseMaterial(10);

    REQUIRE_FALSE(reg.containsMaterial(10));
    REQUIRE(project.uniforms.count(idX) == 0);
    REQUIRE(project.uniforms.count(idY) == 0);
}

TEST_CASE("UniformRegistry: re-registering scene uniform preserves ID", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    Uniform u = makeFloatUniform("sceneLight", 0.f);
    reg.registerSceneUniform(u);
    const auto scenePtr = reg.tryReadSceneUniforms();
    REQUIRE(scenePtr != nullptr);
    const unsigned id = scenePtr->at("sceneLight").ID;

    Uniform u2 = makeFloatUniform("sceneLight", 3.14f);
    reg.registerSceneUniform(u2);

    REQUIRE(reg.tryReadUniform(id) != nullptr);
    REQUIRE(reg.getSceneUniformsSize() == 1);
    REQUIRE(std::get<float>(project.uniforms.at(id).value) == Catch::Approx(3.14f));
}

TEST_CASE("UniformRegistry: re-registering model uniform preserves ID", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    Uniform u = makeFloatUniform("modelU", 1.f);
    reg.registerModelUniform(100, u);
    const auto modelPtr = reg.tryReadModelUniforms(100);
    REQUIRE(modelPtr != nullptr);
    const unsigned id = modelPtr->at("modelU").ID;

    Uniform u2 = makeFloatUniform("modelU", 7.f);
    reg.registerModelUniform(100, u2);

    REQUIRE(reg.getModelUniformsSize(100) == 1);
    REQUIRE(std::get<float>(project.uniforms.at(id).value) == Catch::Approx(7.f));
}

TEST_CASE("UniformRegistry: eraseMaterialUniform removes single entry", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    reg.registerMaterialUniform(2, makeFloatUniform("keep", 1.f));
    reg.registerMaterialUniform(2, makeFloatUniform("drop", 2.f));

    const unsigned dropId = reg.tryReadMaterialUniform(2, "drop")->ID;
    reg.eraseMaterialUniform(2, "drop");

    REQUIRE(reg.containsMaterialUniform(2, "keep"));
    REQUIRE_FALSE(reg.containsMaterialUniform(2, "drop"));
    REQUIRE(project.uniforms.count(dropId) == 0);
}

TEST_CASE("UniformRegistry: registerMaterialUniform stores hasLocation flag", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    Uniform u = makeFloatUniform("uOpacity", 1.f);
    u.hasLocation = false;
    reg.registerMaterialUniform(99, u);

    REQUIRE(reg.tryReadMaterialUniform(99, "uOpacity")->hasLocation == false);
}

TEST_CASE("UniformRegistry: updateUniform can toggle hasLocation (inspector missing-location path)", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    reg.registerMaterialUniform(5, makeFloatUniform("uColor", 1.f));
    const unsigned id = reg.tryReadMaterialUniform(5, "uColor")->ID;

    Uniform copy = *reg.tryReadMaterialUniform(5, "uColor");
    copy.hasLocation = false;
    REQUIRE(reg.updateUniform(id, copy));
    REQUIRE(reg.tryReadMaterialUniform(5, "uColor")->hasLocation == false);

    copy.hasLocation = true;
    REQUIRE(reg.updateUniform(id, copy));
    REQUIRE(reg.tryReadMaterialUniform(5, "uColor")->hasLocation == true);
}

TEST_CASE("UniformRegistry: re-registering material uniform preserves hasLocation from latest register", "[uniform][registry]") {
    Logger logger;
    Project project;
    REQUIRE(initTestLogger(logger));

    UniformRegistry reg;
    REQUIRE(reg.initialize(&logger, &project));

    Uniform a = makeFloatUniform("flag", 0.f);
    a.hasLocation = true;
    reg.registerMaterialUniform(8, a);

    Uniform b = makeFloatUniform("flag", 1.f);
    b.hasLocation = false;
    reg.registerMaterialUniform(8, b);

    REQUIRE(reg.tryReadMaterialUniform(8, "flag")->hasLocation == false);
}
