#pragma once

#include "glm.hpp"

// vec4s are used for alignment reasons only
struct alignas(16) Material_GLSL
{
    glm::vec4 color;
    glm::vec4 emission; // xyz - emission color, w - emission strength
    float smoothness;
};

struct alignas(16) Sphere_GLSL
{
    glm::vec3 center;
    float radius;
    Material_GLSL material;
};

struct alignas(16) Triangle_GLSL
{
    // vec4 used to comply with std430 alignment , w component does nothing
    glm::vec4 v0;
    glm::vec4 v1;
    glm::vec4 v2;
};

struct alignas(16) AABB_GLSL
{
    glm::vec4 min;
    glm::vec4 max;

    inline void GrowToInclude(const glm::vec4& point)
    {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
};

struct alignas(16) Mesh_GLSL
{
    uint32_t TrianglesStartIndex; // Index to the first triangle of the mesh in the triangles buffer
    uint32_t TrianglesCount; // The amount of triangles this mesh consists of in the triangles buffer
    AABB_GLSL aabb;
    Material_GLSL material;
};
