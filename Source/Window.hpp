#pragma once

#include <string>
#include <memory>

class GLFWwindow;

class Window
{
public:
    Window(const size_t width, const size_t height, const std::string& title);
    ~Window();

    void Clear() const;
    void SwapBuffers() const;
    bool ShouldClose() const;

    inline size_t GetWidth() const { return m_Width; }
    inline size_t GetHeight() const { return m_Height; }
    inline GLFWwindow* GetGLFWWindowPtr() const { return m_GLFWWindow; }
private:
    GLFWwindow* m_GLFWWindow;
    size_t m_Width;
    size_t m_Height;
};
