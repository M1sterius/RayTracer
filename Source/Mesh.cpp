#include "Mesh.hpp"
#include "obj_loader.h"
#include "gtx/string_cast.hpp"

Mesh::Mesh(const std::filesystem::path& objPath, const Material_GLSL& material, const glm::mat4& transform)
    : m_Path(objPath), Material(material), AABB({})
{
    auto loader = objl::Loader();

    if (!loader.LoadFile(objPath.string()))
    {
        printf("Unable to find a '.obj' file: %s", objPath.string().c_str());
        return;
    }

    for (size_t i = 0; i < loader.LoadedIndices.size(); i += 3)
    {
        auto v0 = loader.LoadedVertices[loader.LoadedIndices[i]].Position;
        auto v1 = loader.LoadedVertices[loader.LoadedIndices[i + 1]].Position;
        auto v2 = loader.LoadedVertices[loader.LoadedIndices[i + 2]].Position;

        // Terrible temporary implementation of mesh transformation
        m_Triangles.push_back({
          glm::vec4(v0.X, v0.Y, v0.Z, 1.0) * transform,
          glm::vec4(v1.X, v1.Y, v1.Z, 1.0) * transform,
          glm::vec4(v2.X, v2.Y, v2.Z, 1.0) * transform
        });
    }

    for (auto& tri : m_Triangles)
    {
        tri.v0 = tri.v0 * transform;
        tri.v1 = tri.v1 * transform;
        tri.v2 = tri.v2 * transform;

        AABB.GrowToInclude(tri.v0);
        AABB.GrowToInclude(tri.v1);
        AABB.GrowToInclude(tri.v2);
    }
}

Mesh::~Mesh() = default;
