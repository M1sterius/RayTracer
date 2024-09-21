#include "Window.hpp"

#include "glad.h"
#include "glfw3.h"

#include <iostream>

Window::Window(const size_t width, const size_t height, const std::string& title)
    : m_GLFWWindow(nullptr), m_Width(width), m_Height(height)
{
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    m_GLFWWindow = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

    if (!m_GLFWWindow)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_GLFWWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, static_cast<int>(m_Width), static_cast<int>(m_Height));
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glfwSetWindowAttrib(m_GLFWWindow, GLFW_RESIZABLE, GLFW_FALSE);
    glfwSwapInterval(0);
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_GLFWWindow);
}

void Window::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_GLFWWindow);
    glfwPollEvents();
}

