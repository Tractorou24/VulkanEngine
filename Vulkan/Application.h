#pragma once

#include "Device.h"
#include "Model.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"

#include <memory>
#include <vector>

namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Run();
	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void DrawFrame();
		void LoadModels();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);
		void FreeCommandBuffers();

	private:
		Window m_window{ 640, 480, "Hello Vulkan" };
		Device m_device{ m_window };
		std::unique_ptr<Pipeline> m_pipeline;
		std::unique_ptr<SwapChain> m_swapChain;
		std::unique_ptr<Model> m_model;

		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
	};
}
