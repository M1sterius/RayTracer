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

struct alignas(16) Triangle
{
    // vec4 used to comply with std430 alignment , w component does nothing
    glm::vec4 v0;
    glm::vec4 v1;
    glm::vec4 v2;
};

struct alignas(16) Mesh
{
    uint32_t TrianglesStartIndex; // Index to the first triangle of the mesh in the triangles buffer
    uint32_t TrianglesCount; // The amount of triangles this mesh consists of in the triangles buffer
    Material material;
};