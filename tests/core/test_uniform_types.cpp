#include <algorithm>
#include <string>
#include <vector>

#include <catch2/catch_amalgamated.hpp>

#include "core/UniformTypes.hpp"

// Covers UniformTypes.hpp behavior relied on by Apr 13 2026 inspector work:
// - hasLocation on Uniform (bc2aba9)
// - getObjectData(Float) no longer lists getTime (1139170); getTime stays under getSceneVariables(Float).

namespace {

static bool containsString(const std::vector<std::string>& v, const char* s) {
    return std::find(v.begin(), v.end(), std::string(s)) != v.end();
}

} // namespace

TEST_CASE("Uniform: default hasLocation is true", "[uniform][types]") {
    Uniform u;
    REQUIRE(u.hasLocation == true);
}

TEST_CASE("getObjectData: Float has no object-data field list (getTime is scene-only)", "[uniform][types]") {
    REQUIRE_FALSE(getObjectData(UniformType::Float).has_value());
}

TEST_CASE("getObjectData: Vec3 lists position and scale", "[uniform][types]") {
    const auto opt = getObjectData(UniformType::Vec3);
    REQUIRE(opt.has_value());
    REQUIRE(containsString(*opt, "position"));
    REQUIRE(containsString(*opt, "scale"));
}

TEST_CASE("getObjectData: Vec4 lists orientation", "[uniform][types]") {
    const auto opt = getObjectData(UniformType::Vec4);
    REQUIRE(opt.has_value());
    REQUIRE(containsString(*opt, "orientation"));
}

TEST_CASE("getSceneVariables: Float lists getTime", "[uniform][types]") {
    const auto opt = getSceneVariables(UniformType::Float);
    REQUIRE(opt.has_value());
    REQUIRE(containsString(*opt, "getTime"));
}

TEST_CASE("getSceneVariables: Vec3 lists Camera Position", "[uniform][types]") {
    const auto opt = getSceneVariables(UniformType::Vec3);
    REQUIRE(opt.has_value());
    REQUIRE(containsString(*opt, "Camera Position"));
}

TEST_CASE("getSceneVariables: unsupported types return nullopt", "[uniform][types]") {
    REQUIRE_FALSE(getSceneVariables(UniformType::Vec4).has_value());
    REQUIRE_FALSE(getSceneVariables(UniformType::Int).has_value());
}
