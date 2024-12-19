#include "SSBO.hpp"
#include "glad.h"

SSBO::SSBO()
    : m_Handle(0)
{
    glGenBuffers(1, &m_Handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

SSBO::SSBO(const void* data, const size_t size)
    : m_Handle(0)
{
    glGenBuffers(1, &m_Handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

SSBO::~SSBO()
{
    glDeleteBuffers(1, &m_Handle);
}

void SSBO::Bind(const unsigned int slot) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_Handle);
}

void SSBO::Unbind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::UpdateData(const void* data, const size_t size, const size_t offset)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
