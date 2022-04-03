#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <string>
#include <vector>

namespace Engine {

	class SwapChain {
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device& deviceRef, VkExtent2D windowExtent);
		SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
		~SwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain&& operator=(const SwapChain&) = delete;

		VkFramebuffer GetFrameBuffer(int index) { return m_swapChainFramebuffers[index]; }
		VkRenderPass GetRenderPass() { return m_renderPass; }
		VkImageView GetImageView(int index) { return m_swapChainImageViews[index]; }
		size_t ImageCount() { return m_swapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() { return m_swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return m_swapChainExtent; }
		uint32_t Width() { return m_swapChainExtent.width; }
		uint32_t Height() { return m_swapChainExtent.height; }

		float ExtentAspectRatio() { return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height); }
		VkFormat FindDepthFormat();

		VkResult AcquireNextImage(uint32_t* imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();

		// Helper Functions
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		Device& m_device;
		VkExtent2D m_windowExtent;
		VkSwapchainKHR m_swapChain;
		std::shared_ptr<SwapChain> m_oldSwapChain;
		size_t m_currentFrame = 0;

		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkRenderPass m_renderPass;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		std::vector<VkImage> m_depthImages;
		std::vector<VkDeviceMemory> m_depthImageMemorys;
		std::vector<VkImageView> m_depthImageViews;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;

		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		std::vector<VkFence> m_imagesInFlight;
	};

}  // namespace lve
