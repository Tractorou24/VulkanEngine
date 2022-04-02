#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <utility>

namespace Engine
{
	class Window
	{
	public:
		Window(const int& width, const int& height, const std::string& name);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		bool ShouldClose() { return glfwWindowShouldClose(m_window); }
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_windowSize.first), static_cast<uint32_t>(m_windowSize.second) }; }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void InitWindow();

	private:
		std::pair<int, int> m_windowSize;
		std::string m_windowName;
		GLFWwindow* m_window;
	};
}
