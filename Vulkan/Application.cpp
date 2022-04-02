#include "Application.h"

#include <array>
#include <stdexcept>

namespace Engine
{
	Application::Application()
	{
		LoadModels();
		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(m_device.GetDevice(), m_pipelineLayout, nullptr);
	}

	void Application::Run()
	{
		while (!m_window.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}
		vkDeviceWaitIdle(m_device.GetDevice());
	}

	void Application::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout !");
	}

	void Application::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig = {};
		Pipeline::DefaultPipelineConfig(pipelineConfig, std::make_pair(m_swapChain.Width(), m_swapChain.Height()));
		pipelineConfig.renderPass = m_swapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = std::make_unique<Pipeline>(m_device, "shaders\\simple_shader.vert.spv", "shaders\\simple_shader.frag.spv", pipelineConfig);
	}

	void Application::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_swapChain.ImageCount());
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
		allocInfo.pNext = nullptr;

		if (vkAllocateCommandBuffers(m_device.GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers !");

		for (size_t i = 0; i < m_commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
				throw std::runtime_error("Failed to begin recording command buffer !");

			VkRenderPassBeginInfo renderInfo = {};
			renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderInfo.renderPass = m_swapChain.GetRenderPass();
			renderInfo.framebuffer = m_swapChain.GetFrameBuffer(i);
			renderInfo.renderArea.offset = { 0, 0 };
			renderInfo.renderArea.extent = m_swapChain.GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_commandBuffers[i], &renderInfo, VK_SUBPASS_CONTENTS_INLINE);
			m_pipeline->Bind(m_commandBuffers[i]);
			m_model->Bind(m_commandBuffers[i]);
			m_model->Draw(m_commandBuffers[i]);

			vkCmdEndRenderPass(m_commandBuffers[i]);
			if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to record command buffer !");
		}
	}

	void Application::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_swapChain.AcquireNextImage(&imageIndex);

		/*if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			/*m_swapChain.RecreateSwapChain();
			CreateCommandBuffers();
			return;
		}
		else*/ if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image !");

		result = m_swapChain.SubmitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to submit command buffer !");
	}

	void Application::LoadModels()
	{
		std::vector<Model::Vertex> vertices{
			{{0.0f, -0.5f}, {1, 0, 0}},
			{{0.5f, 0.5f}, {0, 1, 0}},
			{{-0.5f, 0.5f}, {0, 0, 1}}
		};

		m_model = std::make_unique<Model>(m_device, vertices);
	}
}
