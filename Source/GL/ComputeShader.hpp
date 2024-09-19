#pragma once

#include <string>
#include <cstdint>

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
private:
    explicit ComputeShader(const std::string& source);

    unsigned int m_Handle;
};

