#include "Mesh.hpp"
#include "obj_loader.h"
#include "gtx/string_cast.hpp"

Mesh::Mesh(const std::filesystem::path& objPath, const Material_GLSL& material, const glm::vec3& pos)
    : m_Path(objPath), Material(material)
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

        // Absolutely terrible temporary solution of mesh translation
        m_Triangles.push_back({
            glm::vec4(v0.X, v0.Y, v0.Z, 0.0) + glm::vec4(pos, 0.0),
            glm::vec4(v1.X, v1.Y, v1.Z, 0.0) + glm::vec4(pos, 0.0),
            glm::vec4(v2.X, v2.Y, v2.Z, 0.0) + glm::vec4(pos, 0.0)
        });
    }

//    for (const auto& tri : m_Triangles)
//    {
//        std::cout << glm::to_string(tri.v0) << glm::to_string(tri.v1) << glm::to_string(tri.v2) << '\n';
//    }
}

Mesh::~Mesh() = default;
