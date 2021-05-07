//
// Created by Mathis Lamidey on 2021-04-11.
//

#include "VkInitialize.h"

namespace MRG::VkInit
{
	vk::CommandPoolCreateInfo cmdPoolCreateInfo(vk::CommandPoolCreateFlags flags, uint32_t queueFamiliyIndex)
	{
		return vk::CommandPoolCreateInfo{.flags = flags, .queueFamilyIndex = queueFamiliyIndex};
	}

	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, vk::CommandBufferLevel level, uint32_t count)
	{
		return vk::CommandBufferAllocateInfo{.commandPool = pool, .level = level, .commandBufferCount = count};
	}

	vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule)
	{
		return vk::PipelineShaderStageCreateInfo{.stage = stage, .module = shaderModule, .pName = "main"};
	}

	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
	{
		// @TODO(Ithyx): fix this when we use it
		return vk::PipelineVertexInputStateCreateInfo{.vertexBindingDescriptionCount = 0, .vertexAttributeDescriptionCount = 0};
	}

	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology topology)
	{
		return vk::PipelineInputAssemblyStateCreateInfo{.topology = topology, .primitiveRestartEnable = VK_FALSE};
	}

	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(vk::PolygonMode polygonMode)
	{
		return vk::PipelineRasterizationStateCreateInfo{.depthClampEnable        = VK_FALSE,
		                                                .rasterizerDiscardEnable = VK_FALSE,
		                                                .polygonMode             = polygonMode,
		                                                .cullMode                = vk::CullModeFlagBits::eBack,
		                                                .frontFace               = vk::FrontFace::eClockwise,
		                                                .depthBiasEnable         = VK_FALSE,
		                                                .lineWidth               = 1.f};
	}

	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo()
	{
		return vk::PipelineMultisampleStateCreateInfo{
		  .rasterizationSamples = vk::SampleCountFlagBits::e1,
		  .sampleShadingEnable  = VK_FALSE,
		};
	}

	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState()
	{
		return vk::PipelineColorBlendAttachmentState{.blendEnable         = VK_TRUE,
		                                             .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
		                                             .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
		                                             .colorBlendOp        = vk::BlendOp::eAdd,
		                                             .srcAlphaBlendFactor = vk::BlendFactor::eOne,
		                                             .dstAlphaBlendFactor = vk::BlendFactor::eZero,
		                                             .alphaBlendOp        = vk::BlendOp::eAdd,
		                                             .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
		                                                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
	}

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo()
	{
		return vk::PipelineLayoutCreateInfo{.setLayoutCount = 0, .pushConstantRangeCount = 0};
	}
}  // namespace MRG::VkInit
