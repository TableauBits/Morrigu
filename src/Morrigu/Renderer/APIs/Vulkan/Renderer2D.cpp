#include "Renderer2D.h"

#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/Renderer2D.h"

namespace
{
	[[nodiscard]] VkPipeline createPipeline(const MRG::Vulkan::WindowProperties* data, MRG::Ref<MRG::Vulkan::Shader> textureShader)
	{
		VkPipeline returnPipeline;

		// TODO: Add proper debug logging for selected rendering features
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = textureShader->m_vertexShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = textureShader->m_fragmentShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(data->swapChain.extent.width);
		viewport.height = static_cast<float>(data->swapChain.extent.height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = data->swapChain.extent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.colorWriteMask =
		  VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = data->pipeline.layout;
		pipelineInfo.renderPass = data->pipeline.renderPass;
		pipelineInfo.subpass = 0;

		MRG_VKVALIDATE(vkCreateGraphicsPipelines(data->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &returnPipeline),
		               "failed to create graphics pipeline!");

		return returnPipeline;
	}

	[[nodiscard]] std::vector<VkFramebuffer> createframeBuffers(const VkDevice device,
	                                                            const std::vector<VkImageView>& swapChainImagesViews,
	                                                            const VkRenderPass renderPass,
	                                                            const VkExtent2D swapChainExtent)
	{
		std::vector<VkFramebuffer> returnFrameBuffers(swapChainImagesViews.size());

		for (std::size_t i = 0; i < swapChainImagesViews.size(); ++i) {
			VkImageView attachments[] = {swapChainImagesViews[i]};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			MRG_VKVALIDATE(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &returnFrameBuffers[i]), "failed to create framebuffer!");
		}

		return returnFrameBuffers;
	}

}  // namespace

namespace MRG::Vulkan
{
	void Renderer2D::init()
	{
		MRG_PROFILE_FUNCTION();

		m_data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(MRG::Renderer2D::getGLFWWindow()));
		m_textureShader = createRef<Shader>("resources/shaders/texture");

		try {
			m_data->pipeline.handle = createPipeline(m_data, m_textureShader);
			MRG_ENGINE_INFO("Vulkan graphics pipeline successfully created");

			m_data->swapChain.frameBuffers = std::move(
			  createframeBuffers(m_data->device, m_data->swapChain.imageViews, m_data->pipeline.renderPass, m_data->swapChain.extent));
			MRG_ENGINE_TRACE("Framebuffers successfully created");
		} catch (const std::runtime_error& e) {
			MRG_ENGINE_ERROR("Vulkan error detected: {}", e.what());
		}
	}

	void Renderer2D::shutdown()
	{
		MRG_PROFILE_FUNCTION();

		for (auto framebuffer : m_data->swapChain.frameBuffers) vkDestroyFramebuffer(m_data->device, framebuffer, nullptr);

		vkDestroyPipeline(m_data->device, m_data->pipeline.handle, nullptr);
		m_textureShader->destroy();
	}

	void Renderer2D::beginScene(const OrthoCamera&) {}

	void Renderer2D::endScene() {}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const glm::vec4&) {}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const Ref<Texture2D>&, float, const glm::vec4&) {}

	void Renderer2D::drawRotatedQuad(const glm::vec3&, const glm::vec2&, float, const glm::vec4&) {}

	void Renderer2D::drawRotatedQuad(const glm::vec3&, const glm::vec2&, float, const Ref<Texture2D>&, float, const glm::vec4&) {}
}  // namespace MRG::Vulkan
