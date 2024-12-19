#pragma once

#include <cstdint>

class SSBO
{
public:
    SSBO();
    SSBO(const void* data, const size_t size);
    ~SSBO();

    void Bind(const unsigned int slot) const;
    void Unbind() const;
    void UpdateData(const void* data, const size_t size, const size_t offset);
private:
    unsigned int m_Handle;
};
