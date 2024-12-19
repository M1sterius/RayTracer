#pragma once

#include "glm.hpp"

struct Material
{
    glm::vec3 color;
    glm::vec3 emissionColor;
    float emissionStrength;
};

struct Sphere
{
    glm::vec3 center;
    float radius;
    Material material;
};
