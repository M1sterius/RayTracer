#pragma once

#include "ComputeShader.hpp"
#include "Stopwatch.hpp"
#include "ShaderStructs.hpp"
#include "SSBO.hpp"

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
    uint32_t RaysPerPixel = 100;

    void AddSphere(Sphere sphere);
private:
    void InitScreenQuad();
    void InitScreenQuadShader();
    void BindForQuadDraw() const;
    void DrawScreenQuad() const;
    void DrawCompute();
    void DrawDebug();

    unsigned int m_ScreenTextureHandle;
    unsigned int m_VertexArrayHandle;
    unsigned int m_IndexBufferHandle;
    unsigned int m_ScreenShaderHandle;

    const char* m_GPUVendor;
    const char* m_DriverVersion;

    std::vector<Sphere> m_Spheres;
    std::unique_ptr<SSBO> m_SSBO;
    bool m_ShouldUpdateSSBO = true;

    Stopwatch m_Stopwatch;
    double m_OldTime;
    double m_DeltaTime;
    uint32_t m_FrameIndex = 0;

    std::unique_ptr<ComputeShader> m_RayTracerShader;
    const Window& m_Window;
};
