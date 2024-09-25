#include "RayTracer.hpp"
#include "Window.hpp"
#include "ShaderSourceProcessing.h"
#include "glad.h"

RayTracer::RayTracer(const Window& window)
    : m_Window(window), m_ScreenTextureHandle(0), m_VertexArrayHandle(0), m_IndexBufferHandle(0),
      m_ScreenShaderHandle(0), m_RayTracerShader(ComputeShader::FromFile("Resources/Shaders/TestCompute.glsl"))
{
    InitScreenQuad();
    InitScreenQuadShader();
    BindForQuadDraw();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &m_ScreenTextureHandle);
}

void RayTracer::Update()
{
    m_RayTracerShader.Bind();
    m_RayTracerShader.Dispatch(m_Window.GetWidth(), m_Window.GetHeight(), 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    DrawScreenQuad();
}

void RayTracer::InitScreenQuad()
{
    /*
     * Create a screen-sized texture that will serve as the output of the compute shader
     */
    glGenTextures(1, &m_ScreenTextureHandle);
    glBindTexture(GL_TEXTURE_2D, m_ScreenTextureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, static_cast<int>(m_Window.GetWidth()), static_cast<int>(m_Window.GetHeight()),
                 0, GL_RGBA, GL_FLOAT,nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*
     * Create a full screen NDC quad to map the output texture onto
     */

    float vertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,   // top-left
        1.0f,  1.0f,   1.0f, 1.0f,   // top-right
        1.0f, -1.0f,   1.0f, 0.0f,   // bottom-right
        -1.0f, -1.0f,   0.0f, 0.0f    // bottom-left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VBO;
    glGenVertexArrays(1, &m_VertexArrayHandle);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &m_IndexBufferHandle);

    glBindVertexArray(m_VertexArrayHandle);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify vertex attribute for position (layout = 0 in shader)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Specify vertex attribute for texture coordinates (layout = 1 in shader)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RayTracer::InitScreenQuadShader()
{
    /*
     * Create a shader that will draw the output texture
     */

    static const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoords;

        out vec2 v_TexCoords;

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            v_TexCoords = aTexCoords;
        }
    )";
    static const char* fragmentSource = R"(
        #version 460 core

        in vec2 v_TexCoords;
        uniform sampler2D u_ScreenTexture;

        out vec4 FragColor;

        void main()
        {
            FragColor = texture(u_ScreenTexture, v_TexCoords);
        }
    )";

    const unsigned int vertexHandle = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexHandle, 1, &vertexSource, nullptr);
    glShaderSource(fragmentHandle, 1, &fragmentSource, nullptr);

    glCompileShader(vertexHandle);
    glCompileShader(fragmentHandle);

    if (GetShaderCompileInfo(vertexHandle) == 1 || GetShaderCompileInfo(fragmentHandle) == 1)
    {
        glDeleteShader(vertexHandle);
        glDeleteShader(fragmentHandle);

        return;
    }

    m_ScreenShaderHandle = glCreateProgram();

    glAttachShader(m_ScreenShaderHandle, vertexHandle);
    glAttachShader(m_ScreenShaderHandle, fragmentHandle);

    glLinkProgram(m_ScreenShaderHandle);

    glValidateProgram(m_ScreenShaderHandle);

    glDeleteShader(vertexHandle);
    glDeleteShader(fragmentHandle);
}

void RayTracer::BindForQuadDraw() const
{
    /*
     * Bind the output texture, quad VAO and EBO and set the shader uniform once
     * because these object bindings do not change between frames
     */

    // Bind the texture to slot 0 for rendering the full screen quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ScreenTextureHandle);

    // Bind the texture to be used as the output of the compute shader
    glBindImageTexture(0, m_ScreenTextureHandle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // Bind the shader and set the uniform
    glUseProgram(m_ScreenShaderHandle);
    const int location = glGetUniformLocation(m_ScreenShaderHandle, "u_ScreenTexture");
    glUniform1i(location, 0);

    glUseProgram(0);

    // Bind VAO and EBO
    glBindVertexArray(m_VertexArrayHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
}

void RayTracer::DrawScreenQuad() const
{
    /*
     * The buffers have already been bound, so we only bind the shader to draw the output texture
     */

    glUseProgram(m_ScreenShaderHandle);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
