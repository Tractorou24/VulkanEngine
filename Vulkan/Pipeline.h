#pragma once

#include "Device.h"

#include <string>
#include <vector>

namespace Engine
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo = {};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
		VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
		std::vector<VkDynamicState> dynamicStateEnables = {};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline(Device& device, const std::string& vertexShaderPath, const std::string fragmentShaderPath, const PipelineConfigInfo& configInfos);
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfig(PipelineConfigInfo& configInfo);

	private:
		void CreateGraphicsPipeline(const std::string& vertexShaderPath, const std::string fragmentShaderPath, const PipelineConfigInfo& configInfos);
		void CreateShaderModule(const std::vector<char> code, VkShaderModule* module);
		static std::vector<char> ReadFile(const std::string& filePath);

	private:
		Device& m_device;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertexShaderModule;
		VkShaderModule m_fragmentShaderModule;
	};
}
