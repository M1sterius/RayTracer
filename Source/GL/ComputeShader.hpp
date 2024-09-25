#pragma once

#include "glm.hpp"

#include <string>
#include <cstdint>
#include <unordered_map>

class ComputeShader
{
public:
    static ComputeShader FromFile(const std::string& path);
    static ComputeShader FromSource(const std::string& source);

    ~ComputeShader();

    inline unsigned int GetHandle() const { return m_Handle; }

    void Bind() const;
    void Unbind() const;

    void Dispatch(const size_t x, const size_t y, const size_t z);

    int FindUniform(const std::string& name) const;

    void SetUniform1is(const std::string& name, const int value) const;
    void SetUniform1i(const std::string& name, const unsigned int value) const;
    void SetUniform1i(const std::string& name, const size_t value) const;
    void SetUniform1f(const std::string& name, const float value) const;
    void SetUniformBool(const std::string& name, const bool value) const;
    void SetUniformVec2(const std::string& name, const glm::vec2& value) const;
    void SetUniformVec3(const std::string& name, const glm::vec3& value) const;
    void SetUniformVec4(const std::string& name, const glm::vec4& value) const;
    void SetUniformMat3(const std::string& name, const bool transpose, const glm::mat3& value) const;
    void SetUniformMat4(const std::string& name, const bool transpose, const glm::mat4& value) const;
private:
    explicit ComputeShader(const std::string& source);

    unsigned int m_Handle;
    mutable std::unordered_map<std::string, int> m_UniformsLocationCache;
};

