#include "Window.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void char_callback(GLFWwindow* window, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(window, c);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void window_focus_callback(GLFWwindow* window, int focused)
{
    glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    if (focused)
    {
        std::cout << "Focused" << std::endl;
        glfwSetKeyCallback(window, key_callback);
        glfwSetCharCallback(window, char_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
}

Window::Window(const char* title, const int width, const int height)
    : m_title(title), m_width(width), m_height(height)
{
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);

    glfwSetWindowFocusCallback(m_window, window_focus_callback);

    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCharCallback(m_window, char_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    
}

Window::~Window()
{
    glfwDestroyWindow(m_window);

}

GLFWwindow* Window::GetWindow()
{
    return m_window;
}