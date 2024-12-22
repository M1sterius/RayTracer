#pragma once

#include "glm.hpp"

// vec4s are used for alignment reasons only
struct alignas(16) Material
{
    glm::vec4 color;
    glm::vec4 emission; // xyz - emission color, w - emission strength
    float smoothness;
};

struct alignas(16) Sphere
{
    glm::vec3 center;
    float radius;
    Material material;
};
