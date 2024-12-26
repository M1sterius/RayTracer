#include "RayTracer.hpp"
#include "Window.hpp"
#include "ShaderSourceProcessing.h"
#include "Random.hpp"
#include "gtx/string_cast.hpp"
#include "glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

RayTracer::RayTracer(const Window& window)
    : m_Window(window), m_ScreenTextureHandle(0), m_VertexArrayHandle(0), m_IndexBufferHandle(0),
      m_ScreenShaderHandle(0)
{
    InitScreenQuad();
    InitScreenQuadShader();
    BindForQuadDraw();

    m_RayTracerShader = std::unique_ptr<ComputeShader>(ComputeShader::FromFilePtr("Resources/Shaders/RayTracingShader.glsl"));
    m_SSBO = std::make_unique<SSBO>();
    m_SSBO->BindToSlot(1);

    m_Stopwatch = Stopwatch::StartNew();
    m_OldTime = 0;
    m_DeltaTime = 0;

    m_CamPosition = glm::vec3(0.0);
    m_CamRotation = glm::identity<glm::quat>();
    m_CamViewMatrix = glm::identity<glm::mat4>();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &m_ScreenTextureHandle);
}

void RayTracer::Update()
{
    const auto time = m_Stopwatch.GetElapsed().AsSeconds();
    m_DeltaTime = time - m_OldTime;
    m_OldTime = time;
    m_FrameIndex++;

    DrawCompute();
    DrawScreenQuad();
    DrawDebug();
}

void RayTracer::AddSphere(Sphere sphere)
{
    m_Spheres.push_back(sphere);
    m_SSBO->UpdateData(m_Spheres.data(), m_Spheres.size() * sizeof(Sphere), 0);
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
    glGenBuffers(1, &m_IndexBufferHandle);
    glGenBuffers(1, &VBO);

    glBindVertexArray(m_VertexArrayHandle);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Specify vertex attribute for position (layout = 0 in shader)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // Specify vertex attribute for texture coordinates (layout = 1 in shader)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

void RayTracer::DrawCompute()
{
    m_RayTracerShader->Bind();

    m_RayTracerShader->SetUniformVec2("u_ScreenSize", glm::vec2(m_Window.GetWidth(), m_Window.GetHeight()));
    m_RayTracerShader->SetUniform1i("u_MaxReflectionsCount", MaxReflectionsCount);
    m_RayTracerShader->SetUniform1i("u_RaysPerPixel", RaysPerPixel);
    m_RayTracerShader->SetUniform1i("u_FrameIndex", m_FrameIndex);
//    m_RayTracerShader->SetUniform1i("u_RandomSeed", GenerateRandomUint64(0, 0xFFFFFFFF));
    m_RayTracerShader->SetUniform1i("u_SSBOSpheresCount", m_Spheres.size());

    m_RayTracerShader->SetUniformVec3("u_CameraPosition", m_CamPosition);
    m_RayTracerShader->SetUniformVec3("u_CameraForward", glm::vec4(camForward) * m_CamViewMatrix);
    m_RayTracerShader->SetUniformVec3("u_CameraUp", glm::vec4(camUp) * m_CamViewMatrix);
    m_RayTracerShader->SetUniformVec3("u_CameraRight", glm::vec4(camRight) * m_CamViewMatrix);

    m_RayTracerShader->Dispatch(m_Window.GetWidth() / 8, m_Window.GetHeight() / 8, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void RayTracer::DrawDebug()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const auto fpsText = "FPS: " + std::to_string(1 / m_DeltaTime);
    ImGui::Text("%s", fpsText.c_str());

    int IntMaxReflectionsCount = MaxReflectionsCount;
    ImGui::SliderInt("Max ray reflections", &IntMaxReflectionsCount, 1, 100);
    MaxReflectionsCount = IntMaxReflectionsCount;

    int IntRaysPerPixel = RaysPerPixel;
    ImGui::SliderInt("Rays per pixel", &IntRaysPerPixel, 1, 100);
    RaysPerPixel = IntRaysPerPixel;

    ImGui::SliderFloat("Cam FOV", &FOV, 0, glm::pi<float>() / 2);
    ImGui::SliderFloat("Focal length", &FocalLength, 0.001, 10);

    const auto camPosTxt = "Camera Position: " + glm::to_string(m_CamPosition);
    ImGui::Text("%s", camPosTxt.c_str());
    const auto camRotTxt = "Camera Rotation: " + glm::to_string(glm::degrees(glm::eulerAngles(m_CamRotation)));
    ImGui::Text("%s", camRotTxt.c_str());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RayTracer::SetCamPosition(const glm::vec3& pos)
{
    m_CamPosition = pos;
    UpdateCamTransform();
}

void RayTracer::SetCamRotation(const glm::quat& rot)
{
    m_CamRotation = rot;
    UpdateCamTransform();
}

void RayTracer::UpdateCamTransform()
{
    m_CamViewMatrix = glm::mat4_cast(m_CamRotation);
    m_CamViewMatrix = glm::translate(m_CamViewMatrix, m_CamPosition);
}
