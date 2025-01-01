#pragma once

#include "ShaderStructs.hpp"

#include <filesystem>
#include <vector>

class Mesh
{
public:
    Mesh(const std::filesystem::path& objPath, const Material_GLSL& material, const glm::mat4& transform);
    ~Mesh();

    Mesh(const Mesh&) = default;
    Mesh& operator =(const Mesh&) = default;

    Material_GLSL Material;
    AABB_GLSL AABB;

    const std::vector<Triangle_GLSL>& GetTriangles() const { return m_Triangles; }
private:
    std::vector<Triangle_GLSL> m_Triangles;
    std::filesystem::path m_Path;
};
