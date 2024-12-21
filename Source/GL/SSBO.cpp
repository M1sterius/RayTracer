#include "SSBO.hpp"
#include "glad.h"

#include <iostream>

SSBO::SSBO()
    : m_Handle(0)
{
    glGenBuffers(1, &m_Handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(DefaultInitialBufferSize),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

SSBO::SSBO(const void* data, const size_t size)
    : m_Handle(0)
{
    glGenBuffers(1, &m_Handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), (GLvoid*)data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

SSBO::~SSBO()
{
    glDeleteBuffers(1, &m_Handle);
}

void SSBO::BindToSlot(const unsigned int slot) const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_Handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::Unbind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SSBO::UpdateData(const void* data, const size_t size, const size_t offset) const
{
    // Limited by the size of initial glBufferData memory allocation
    // To resize the buffer use new glBufferData call with the new size (the data won't be automatically copied tho)

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(offset),
                    static_cast<GLsizeiptr>(size), data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
