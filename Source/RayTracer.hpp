#pragma once

class Window;

class RayTracer
{
public:
    explicit RayTracer(const Window& window);
    ~RayTracer();

    void Update() const;
private:
    void InitScreenQuad();
    void InitScreenQuadShader();
    void DrawScreenQuad() const;

    unsigned int m_ScreenTextureHandle;
    unsigned int m_VertexArrayHandle;
    unsigned int m_IndexBufferHandle;
    unsigned int m_ScreenShaderHandle;

    const Window& m_Window;
};
