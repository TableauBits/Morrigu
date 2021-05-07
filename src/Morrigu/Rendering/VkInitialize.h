//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_VKINITIALIZE_H
#define MORRIGU_VKINITIALIZE_H

#include "Rendering/VkTypes.h"

namespace MRG::VkInit
{
	vk::CommandPoolCreateInfo cmdPoolCreateInfo(vk::CommandPoolCreateFlags flags, uint32_t queueFamiliyIndex);
	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, vk::CommandBufferLevel level, uint32_t count);

	vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule);
	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo();
	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology topology);
	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(vk::PolygonMode polygonMode);
	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo();
	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState();

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo();
}  // namespace MRG::VkInit

#endif  // MORRIGU_VKINITIALIZE_H
