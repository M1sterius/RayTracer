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
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &m_ScreenTextureHandle);
}

void RayTracer::Update() const
{
    DrawScreenQuad();
}

void RayTracer::InitScreenQuad()
{
    // Init screen sized texture
    glGenTextures(1, &m_ScreenTextureHandle);
    glBindTexture(GL_TEXTURE_2D, m_ScreenTextureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Window.GetWidth(), m_Window.GetHeight(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE,nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Init screen NDC quad
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

    // Create buffers and VAO
    unsigned int VBO;
    glGenVertexArrays(1, &m_VertexArrayHandle);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &m_IndexBufferHandle);

    // Bind VAO
    glBindVertexArray(m_VertexArrayHandle);

    // Bind and set vertex buffer (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set index buffer (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify vertex attribute for position (layout = 0 in shader)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Specify vertex attribute for texture coordinates (layout = 1 in shader)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void RayTracer::InitScreenQuadShader()
{
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

    // Bind the shader and set the uniforms once since there are no changes between frames
    glUseProgram(m_ScreenShaderHandle);
    const int location = glGetUniformLocation(m_ScreenShaderHandle, "u_ScreenTexture");
    glUniform1i(location, 0);

    // Bind the texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ScreenTextureHandle);
}

void RayTracer::DrawScreenQuad() const
{
    /*
     * All buffers and shaders have been bound during initialisation because there are
     * no changes between frames.
     */

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
