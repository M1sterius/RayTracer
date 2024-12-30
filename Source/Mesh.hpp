#pragma once

#include "ShaderStructs.hpp"

#include <filesystem>
#include <vector>

class Mesh
{
public:
    Mesh(const std::filesystem::path& objPath, const Material_GLSL& material, const glm::vec3& pos);
    ~Mesh();

    Material_GLSL Material;
private:
    std::vector<Triangle_GLSL> m_Triangles;
    std::filesystem::path m_Path;
};
