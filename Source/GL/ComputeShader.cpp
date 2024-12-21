#include "ComputeShader.hpp"
#include "ShaderSourceProcessing.h"
#include "GLSLPreprocessor.h"
#include "glad.h"
#include "gtc/type_ptr.hpp"

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
    const std::string source = ProcessShader(path, "Resources/Shaders");
    return ComputeShader(source);
}

ComputeShader ComputeShader::FromSource(const std::string& source)
{
    return ComputeShader(source);
}

ComputeShader* ComputeShader::FromFilePtr(const std::string &path)
{
    const std::string source = ProcessShader(path, "Resources/Shaders");
    return new ComputeShader(source);
}

ComputeShader* ComputeShader::FromSourcePtr(const std::string &source)
{
    return new ComputeShader(source);
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
}

int ComputeShader::FindUniform(const std::string &name) const
{
    if (m_UniformsLocationCache.find(name) != m_UniformsLocationCache.end())
        return m_UniformsLocationCache[name];

    const int location = glGetUniformLocation(m_Handle, name.c_str());
    if (location != -1)
    {
        m_UniformsLocationCache[name] = location;
        return location;
    }
    std::cout << "Unable to find uniform named: " << name << '\n';
    return -1;
}

void ComputeShader::SetUniform1is(const std::string& name, const int value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform1i(location, value);
}

void ComputeShader::SetUniform1i(const std::string& name, const unsigned int value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform1ui(location, value);
}

void ComputeShader::SetUniform1i(const std::string& name, const size_t value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform1ui(location, value);
}

void ComputeShader::SetUniform1f(const std::string& name, const float value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform1f(location, value);
}

void ComputeShader::SetUniformBool(const std::string &name, const bool value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform1i(location, value);
}

void ComputeShader::SetUniformVec2(const std::string& name, const glm::vec2& value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform2f(location, value.x, value.y);
}

void ComputeShader::SetUniformVec3(const std::string& name, const glm::vec3& value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform3f(location, value.x, value.y, value.z);
}

void ComputeShader::SetUniformVec4(const std::string& name, const glm::vec4& value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void ComputeShader::SetUniformMat3(const std::string& name, const bool transpose, const glm::mat3& value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniformMatrix3fv(location, 1, transpose, glm::value_ptr(value));
}

void ComputeShader::SetUniformMat4(const std::string& name, const bool transpose, const glm::mat4& value) const
{
    const int location = FindUniform(name);
    if (location == -1) return;
    glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(value));
}
