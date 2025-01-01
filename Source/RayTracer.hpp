#pragma once

#include "ComputeShader.hpp"
#include "Stopwatch.hpp"
#include "ShaderStructs.hpp"
#include "SSBO.hpp"
#include "Mesh.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"

#include <memory>
#include <vector>

class Window;

class RayTracer
{
public:
    explicit RayTracer(const Window& window);
    ~RayTracer();

    void Update();

    uint32_t MaxReflectionsCount = 10;
    uint32_t RaysPerPixel = 32;
    float FOV = 3.14159 / 2;
    float FocalLength = 1.0;

    void AddMesh(const Mesh& mesh);
    void SetCamPosition(const glm::vec3& pos);
    void SetCamRotation(const glm::quat& rot);
private:
    void InitScreenQuad();
    void InitScreenQuadShader();
    void BindForQuadDraw() const;
    void DrawScreenQuad() const;
    void DrawCompute();
    void DrawDebug();
    void UpdateCamTransform();

    unsigned int m_ScreenTextureHandle;
    unsigned int m_VertexArrayHandle;
    unsigned int m_IndexBufferHandle;
    unsigned int m_ScreenShaderHandle;

    std::vector<Mesh> m_Meshes;
    std::unique_ptr<SSBO> m_SSBO;

    Stopwatch m_Stopwatch;
    double m_OldTime;
    double m_DeltaTime;
    uint32_t m_FrameIndex = 0;

    std::unique_ptr<ComputeShader> m_RayTracerShader;
    const Window& m_Window;

    glm::vec3 m_CamPosition;
    glm::quat m_CamRotation;
    glm::mat4 m_CamViewMatrix;

    static constexpr auto camForward = glm::vec4(0.0, 0.0, -1.0, 0.0);
    static constexpr auto camUp = glm::vec4(0.0, 1.0, 0.0, 0.0);
    static constexpr auto camRight = glm::vec4(1.0, 0.0, 0.0, 0.0);

    static constexpr uint32_t MESH_COUNT_LIMIT = 512;
};
