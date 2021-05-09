//
// Created by Mathis Lamidey on 2021-04-15.
//

#include "PipelineBuilder.h"

namespace MRG
{
	vk::Pipeline PipelineBuilder::build_pipeline(const vk::Device& device, const vk::RenderPass& renderPass)
	{
		vk::PipelineViewportStateCreateInfo viewportStateInfo{.viewportCount = 1, .scissorCount = 1};

		vk::PipelineColorBlendStateCreateInfo colorBlendInfo{
		  .logicOpEnable = VK_FALSE, .attachmentCount = 1, .pAttachments = &colorBlendAttachment};

		std::array<vk::DynamicState, 2> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
		vk::PipelineDynamicStateCreateInfo dynamicStatesInfo{.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
		                                                     .pDynamicStates    = dynamicStates.data()};

		vk::GraphicsPipelineCreateInfo pipelineInfo{.stageCount          = static_cast<uint32_t>(shaderStages.size()),
		                                            .pStages             = shaderStages.data(),
		                                            .pVertexInputState   = &vertexInputInfo,
		                                            .pInputAssemblyState = &inputAssemblyInfo,
		                                            .pViewportState      = &viewportStateInfo,
		                                            .pRasterizationState = &rasterizerInfo,
		                                            .pMultisampleState   = &multisamplingInfo,
		                                            .pColorBlendState    = &colorBlendInfo,
		                                            .pDynamicState       = &dynamicStatesInfo,
		                                            .layout              = pipelineLayout,
		                                            .renderPass          = renderPass,
		                                            .subpass             = 0};
		const auto result = device.createGraphicsPipeline(pipelineCache, pipelineInfo);
		MRG_VK_CHECK_HPP(result.result, "Failed to create graphics pipeline!")

		return result.value;
	}
}  // namespace MRG