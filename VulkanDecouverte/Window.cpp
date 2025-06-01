#include "Window.h"

Window::Window(const char* title, const int width, const int height)
    : m_title(title), m_width(width), m_height(height)
{
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);

}

Window::~Window()
{
    glfwDestroyWindow(m_window);

}

GLFWwindow* Window::GetWindow()
{
    return m_window;
}
