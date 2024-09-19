#pragma once

#include <string>

class GLFWwindow;

class Window
{
public:
    Window(const size_t width, const size_t height, const std::string& title);
    ~Window();

    void Update() const;
    bool ShouldClose() const;
private:
    GLFWwindow* m_GLFWWindow;
    size_t m_Width;
    size_t m_Height;
};

