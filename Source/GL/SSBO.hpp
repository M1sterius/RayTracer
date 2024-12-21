#pragma once

#include <cstdint>

class SSBO
{
public:
    SSBO();
    SSBO(const void* data, const size_t size);
    ~SSBO();

    size_t DefaultInitialBufferSize = 1 * 1024 * 1024; // 1 MB

    inline unsigned int GetHandle() const { return m_Handle; }

    void BindToSlot(const unsigned int slot) const;
    void Unbind() const;
    void UpdateData(const void* data, const size_t size, const size_t offset) const;
private:
    unsigned int m_Handle;
};
