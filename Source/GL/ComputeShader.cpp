#include "ComputeShader.hpp"
#include "ShaderSourceProcessing.h"
#include "glad.h"

ComputeShader::ComputeShader(const std::string& source)
    : m_Handle(0)
{
    const GLchar* src = source.c_str();

    const unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &src, nullptr);
    glCompileShader(computeShader);

    if (GetShaderCompileInfo(computeShader))
    {
        glDeleteShader(computeShader);
    }

    m_Handle = glCreateProgram();
    glAttachShader(m_Handle, computeShader);
    glLinkProgram(m_Handle);
}

ComputeShader ComputeShader::FromFile(const std::string& path)
{
    const std::string source = ProcessShaderSource(path);
    return ComputeShader(source);
}

ComputeShader ComputeShader::FromSource(const std::string& source)
{
    return ComputeShader(source);
}

ComputeShader::~ComputeShader()
{
    glDeleteProgram(m_Handle);
}

void ComputeShader::Bind() const
{
    glUseProgram(m_Handle);
}

void ComputeShader::Unbind() const
{
    glUseProgram(0);
}

void ComputeShader::Dispatch(const size_t x, const size_t y, const size_t z)
{
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
