#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
};