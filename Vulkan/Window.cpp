#include "Window.h"

#include <stdexcept>

namespace Engine
{
	Window::Window(const int& width, const int& height, const std::string& name)
		: m_windowSize(std::make_pair(width, height)), m_windowName(name)
	{
		InitWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface !");
	}

	void Window::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_windowSize.first, m_windowSize.second, m_windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
	}

	void Window::FramebufferResizeCallback(GLFWwindow* GLFWwindow, int width, int height)
	{
		Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(GLFWwindow));
		window->m_frameBufferResized = true;
		window->m_windowSize = std::make_pair(width, height);
	}
}
