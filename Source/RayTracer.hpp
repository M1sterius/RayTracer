#pragma once

#include "ComputeShader.hpp"
#include "Stopwatch.h"

#include <memory>

class Window;

class RayTracer
{
public:
    explicit RayTracer(const Window& window);
    ~RayTracer();

    void Update();

    uint32_t MaxReflectionsCount = 4;
    uint32_t RaysPerPixel = 1;
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

    Stopwatch m_Stopwatch;
    double m_OldTime;
    double m_DeltaTime;

    ComputeShader m_RayTracerShader;
    const Window& m_Window;
};
