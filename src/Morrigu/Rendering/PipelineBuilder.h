//
// Created by Mathis Lamidey on 2021-04-15.
//

#ifndef MORRIGU_PIPELINEBUILDER_H
#define MORRIGU_PIPELINEBUILDER_H

#include "Rendering/VkTypes.h"

#include <vector>

namespace MRG
{
	class PipelineBuilder
	{
	public:
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		vk::PipelineRasterizationStateCreateInfo rasterizerInfo{};
		vk::PipelineMultisampleStateCreateInfo multisamplingInfo{};
		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		vk::PipelineLayout pipelineLayout{};
		vk::PipelineCache pipelineCache{};

		[[nodiscard]] vk::Pipeline build_pipeline(const vk::Device& device, const vk::RenderPass& renderPass);
	};
}  // namespace MRG

#endif  // MORRIGU_PIPELINEBUILDER_H
