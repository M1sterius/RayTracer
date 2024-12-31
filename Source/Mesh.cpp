#include "Mesh.hpp"
#include "obj_loader.h"
#include "gtx/string_cast.hpp"

Mesh::Mesh(const std::filesystem::path& objPath, const Material_GLSL& material, const glm::mat4& transform)
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

//        // Terrible temporary implementation of mesh transformation
//        m_Triangles.push_back({
//            glm::vec4(v0.X, v0.Y, v0.Z, 1.0) * transform,
//            glm::vec4(v1.X, v1.Y, v1.Z, 1.0) * transform,
//            glm::vec4(v2.X, v2.Y, v2.Z, 1.0) * transform
//        });

        glm::vec4 v0Transformed = transform * glm::vec4(v0.X, v0.Y, v0.Z, 1.0);
        glm::vec4 v1Transformed = transform * glm::vec4(v1.X, v1.Y, v1.Z, 1.0);
        glm::vec4 v2Transformed = transform * glm::vec4(v2.X, v2.Y, v2.Z, 1.0);

// If w is not 1.0, normalize to ensure correctness
        v0Transformed /= v0Transformed.w;
        v1Transformed /= v1Transformed.w;
        v2Transformed /= v2Transformed.w;

        m_Triangles.push_back({
              glm::vec4(v0Transformed.x, v0Transformed.y, v0Transformed.z, 1.0), // Convert back to vec3 if needed
              glm::vec4(v1Transformed.x, v1Transformed.y, v1Transformed.z, 1.0),
              glm::vec4(v2Transformed.x, v2Transformed.y, v2Transformed.z, 1.0)
        });
    }

//    for (const auto& tri : m_Triangles)
//    {
//        std::cout << glm::to_string(tri.v0) << glm::to_string(tri.v1) << glm::to_string(tri.v2) << '\n';
//    }
}

Mesh::~Mesh() = default;
