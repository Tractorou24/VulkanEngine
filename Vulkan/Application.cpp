#include "Application.h"

#include <array>
#include <stdexcept>

namespace Engine
{
	Application::Application()
	{
		LoadModels();
		CreatePipelineLayout();
		RecreateSwapChain();
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
		assert(m_swapChain != nullptr && "Cannot create swap chain before swap chain !");
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout !");

		PipelineConfigInfo pipelineConfig = {};
		Pipeline::DefaultPipelineConfig(pipelineConfig);
		pipelineConfig.renderPass = m_swapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = std::make_unique<Pipeline>(m_device, "shaders\\simple_shader.vert.spv", "shaders\\simple_shader.frag.spv", pipelineConfig);
	}

	void Application::CreateCommandBuffers()
	{
		m_commandBuffers.resize(m_swapChain->ImageCount());
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());
		allocInfo.pNext = nullptr;

		if (vkAllocateCommandBuffers(m_device.GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers !");
	}

	void Application::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_swapChain->AcquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image !");

		RecordCommandBuffer(imageIndex);
		result = m_swapChain->SubmitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.HasWindowResized())
		{
			m_window.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}

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

	void Application::RecreateSwapChain()
	{
		auto extent = m_window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_window.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_device.GetDevice());
		if (m_swapChain == nullptr)
		{
			m_swapChain = std::make_unique<SwapChain>(m_device, extent);
		}
		else
		{
			m_swapChain = std::make_unique<SwapChain>(m_device, extent, std::move(m_swapChain));
			if (m_swapChain->ImageCount() != m_commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		// FIXME : Check if render passes are compatible
		CreatePipeline();
	}

	void Application::RecordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer !");

		VkRenderPassBeginInfo renderInfo = {};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderInfo.renderPass = m_swapChain->GetRenderPass();
		renderInfo.framebuffer = m_swapChain->GetFrameBuffer(imageIndex);
		renderInfo.renderArea.offset = { 0, 0 };
		renderInfo.renderArea.extent = m_swapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewportInfo = {};
		viewportInfo.x = 0;
		viewportInfo.y = 0;
		viewportInfo.width = static_cast<float>(m_swapChain->GetSwapChainExtent().width);
		viewportInfo.height = static_cast<float>(m_swapChain->GetSwapChainExtent().height);
		viewportInfo.minDepth = 0.0f;
		viewportInfo.maxDepth = 1.0f;

		VkRect2D scissorInfo = {};
		scissorInfo.offset = { 0, 0 };
		scissorInfo.extent = m_swapChain->GetSwapChainExtent();

		vkCmdSetViewport(m_commandBuffers[imageIndex], 0, 1, &viewportInfo);
		vkCmdSetScissor(m_commandBuffers[imageIndex], 0, 1, &scissorInfo);

		m_pipeline->Bind(m_commandBuffers[imageIndex]);
		m_model->Bind(m_commandBuffers[imageIndex]);
		m_model->Draw(m_commandBuffers[imageIndex]);

		vkCmdEndRenderPass(m_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer !");
	}

	void Application::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_device.GetDevice(), m_device.GetCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
	}
}
