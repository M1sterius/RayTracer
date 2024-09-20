#pragma once

#include <string>

class GLFWwindow;

class Window
{
public:
    static Window& Init(const size_t width, const size_t height, const std::string& title);

    void Update() const;
    bool ShouldClose() const;
private:
    Window(const size_t width, const size_t height, const std::string& title);
    ~Window();

    GLFWwindow* m_GLFWWindow;
    size_t m_Width;
    size_t m_Height;
};

