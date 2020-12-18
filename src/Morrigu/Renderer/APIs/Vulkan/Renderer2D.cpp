#include "Renderer2D.h"

#include "Core/GLMIncludeHelper.h"
#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/APIs/Vulkan/VertexArray.h"

#include <ImGui/bindings/imgui_impl_vulkan.h>
#include <imgui.h>

#include <array>

namespace
{
	[[nodiscard]] MRG::Vulkan::QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface)
	{
		MRG::Vulkan::QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete())
				break;
			++i;
		}

		return indices;
	}

	[[nodiscard]] VkSurfaceFormatKHR chooseSwapFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		for (const auto& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		// In case we don't find exactly what we are searching for, we just select the first one.
		// This is guaranteed to exist because of the isDeviceSuitable check beforehand
		// TODO: update this function to better select surface formats
		return formats[0];
	}

	[[nodiscard]] VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
	{
		// If possible, prefer triple buffering
		for (const auto& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentMode;
		}

		// This mode is guaranteed to be present by the specs
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	[[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const MRG::WindowProperties* data)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) {
			// We don't need to change anything in this case !
			return capabilities.currentExtent;
		}

		// Otherwise, we have to provide the best values possible
		VkExtent2D actualExtent{data->width, data->height};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

	[[nodiscard]] std::vector<VkImageView> createimageViews(const VkDevice device, const std::vector<VkImage>& images, VkFormat imageFormat)
	{
		std::vector<VkImageView> imageViews(images.size());
		for (std::size_t i = 0; i < images.size(); i++) {
			imageViews[i] = MRG::Vulkan::createImageView(device, images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		return imageViews;
	}

	[[nodiscard]] MRG::Vulkan::SwapChain createSwapChain(const VkPhysicalDevice physicalDevice,
	                                                     const VkSurfaceKHR surface,
	                                                     const VkDevice device,
	                                                     const MRG::WindowProperties* data)
	{
		VkSwapchainKHR handle{};
		MRG::Vulkan::SwapChainSupportDetails SwapChainSupport = MRG::Vulkan::querySwapChainSupport(physicalDevice, surface);

		const auto surfaceFormat = chooseSwapFormat(SwapChainSupport.formats);
		const auto presentMode = chooseSwapPresentMode(SwapChainSupport.presentModes);
		const auto extent = chooseSwapExtent(SwapChainSupport.capabilities, data);

		auto imageCount = SwapChainSupport.capabilities.minImageCount + 1;
		if (SwapChainSupport.capabilities.maxImageCount > 0 && imageCount > SwapChainSupport.capabilities.maxImageCount)
			imageCount = SwapChainSupport.capabilities.maxImageCount;

		MRG::Vulkan::QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		uint32_t minImageCount = imageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		if (indices.graphicsFamily != indices.presentFamily) {
			// The queues are not the same!
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			// The queues are the same, no need to separated them!
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		createInfo.preTransform = SwapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		MRG_VKVALIDATE(vkCreateSwapchainKHR(device, &createInfo, nullptr, &handle), "failed to create swapChain!");

		vkGetSwapchainImagesKHR(device, handle, &imageCount, nullptr);
		std::vector<VkImage> images(imageCount);
		vkGetSwapchainImagesKHR(device, handle, &imageCount, images.data());

		const auto imageViews = createimageViews(device, images, surfaceFormat.format);

		return {handle, minImageCount, imageCount, images, surfaceFormat.format, extent, imageViews, {}, {}};
	}

	[[nodiscard]] VkDescriptorSetLayout createDescriptorSetLayout(const VkDevice device, uint32_t textureSlotCount)
	{
		VkDescriptorSetLayout returnLayout;

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = textureSlotCount;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		MRG_VKVALIDATE(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &returnLayout), "failed to create descriptor set layout!");

		return returnLayout;
	}

	[[nodiscard]] VkPushConstantRange populatePushConstantsRanges()
	{
		VkPushConstantRange pushConstantsRange{};
		pushConstantsRange.offset = 0;
		pushConstantsRange.size = sizeof(MRG::Vulkan::PushConstants);
		pushConstantsRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		return pushConstantsRange;
	}

	[[nodiscard]] VkPipelineLayoutCreateInfo populatePipelineLayout(const VkDescriptorSetLayout* descriptorSetLayout,
	                                                                const VkPushConstantRange* pushConstantsRange)
	{
		VkPipelineLayoutCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineCreateInfo.setLayoutCount = 1;
		pipelineCreateInfo.pSetLayouts = descriptorSetLayout;
		pipelineCreateInfo.pushConstantRangeCount = 1;
		pipelineCreateInfo.pPushConstantRanges = pushConstantsRange;

		return pipelineCreateInfo;
	}

	VkFormat findSupportedFormat(const VkPhysicalDevice physicalDevice,
	                             const std::vector<VkFormat>& candidates,
	                             VkImageTiling tiling,
	                             VkFormatFeatureFlags features)
	{
		for (const auto& format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		MRG_CORE_ASSERT(false, "failed to find a supported format!");
		return VK_FORMAT_MAX_ENUM;
	}

	auto findDepthFormat(const VkPhysicalDevice physicalDevice)
	{
		return findSupportedFormat(physicalDevice,
		                           {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		                           VK_IMAGE_TILING_OPTIMAL,
		                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	[[nodiscard]] std::array<VkRenderPass, 3>
	createRenderPasses(const VkPhysicalDevice physicalDevice, VkDevice device, VkFormat swapChainFormat)
	{
		/// We need 3 render passes:
		// - A clearing render pass
		// - A rendering render pass, that will be used once per scene (or batch), and will flush a batch;
		// - An ImGui render pass to ensure that it's drawn last.
		std::array<VkRenderPass, 3> renderpasses{};

		std::array<VkAttachmentDescription, 3> colorAttachments{};
		colorAttachments[0].format = swapChainFormat;
		colorAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		colorAttachments[1].format = swapChainFormat;
		colorAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachments[1].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[1].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		colorAttachments[2].format = swapChainFormat;
		colorAttachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachments[2].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// The clearing subpass needs to clear the z buffer, and the rendering subpass needs to load it instead.
		// ImGui doesn't care
		std::array<VkAttachmentDescription, 3> depthAttachments{};
		const auto format = findDepthFormat(physicalDevice);
		depthAttachments[0].format = format;
		depthAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachments[0].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		depthAttachments[1].format = format;
		depthAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		depthAttachments[2].format = format;
		depthAttachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachments[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachments[2].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachments[2].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments;
		VkRenderPassCreateInfo renderPassInfo{};

		for (std::size_t i = 0; i < 3; ++i) {
			attachments = {colorAttachments[i], depthAttachments[i]};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderPassInfo.pAttachments = attachments.data();
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;
			MRG_VKVALIDATE(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderpasses[i]), "failed to create render passes!");
		}

		return renderpasses;
	}

	[[nodiscard]] std::array<VkPipeline, 3> createPipelines(const MRG::Vulkan::WindowProperties* data,
	                                                        MRG::Ref<MRG::Vulkan::Shader> textureShader,
	                                                        const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions,
	                                                        const std::vector<VkVertexInputBindingDescription>& bindingDescriptions)
	{
		std::array<VkPipeline, 3> pipelines;

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
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkDynamicState dynamicStates[2]{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = nullptr;  // Dynamic state
		viewportState.scissorCount = 1;
		viewportState.pScissors = nullptr;  // Dynamic state

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

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
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = data->clearingPipeline.layout;
		pipelineInfo.renderPass = data->clearingPipeline.renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.pDynamicState = &dynamicState;
		MRG_VKVALIDATE(vkCreateGraphicsPipelines(data->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelines[0]),
		               "failed to create graphics pipeline!");

		pipelineInfo.layout = data->renderingPipeline.layout;
		pipelineInfo.renderPass = data->renderingPipeline.renderPass;
		MRG_VKVALIDATE(vkCreateGraphicsPipelines(data->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelines[1]),
		               "failed to create graphics pipeline!");

		pipelineInfo.layout = data->ImGuiPipeline.layout;
		pipelineInfo.renderPass = data->ImGuiPipeline.renderPass;
		MRG_VKVALIDATE(vkCreateGraphicsPipelines(data->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelines[2]),
		               "failed to create graphics pipeline!");

		return pipelines;
	}

	[[nodiscard]] std::vector<std::array<VkFramebuffer, 3>> createFramebuffers(const VkDevice device,
	                                                                           const std::vector<VkImageView>& swapChainImagesViews,
	                                                                           const VkImageView depthImageView,
	                                                                           const std::array<VkRenderPass, 3>& renderPasses,
	                                                                           const VkExtent2D swapChainExtent)
	{
		std::vector<std::array<VkFramebuffer, 3>> frameBuffers(swapChainImagesViews.size());

		for (std::size_t i = 0; i < swapChainImagesViews.size(); ++i) {
			std::array<VkImageView, 2> attachments = {swapChainImagesViews[i], depthImageView};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			for (std::size_t j = 0; j < 3; ++j) {
				framebufferInfo.renderPass = renderPasses[j];
				MRG_VKVALIDATE(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i][j]),
				               "failed to create framebuffer!");
			}
		}

		return frameBuffers;
	}

	[[nodiscard]] VkCommandPool createCommandPool(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface)
	{
		VkCommandPool returnCommandPool;

		const auto queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		MRG_VKVALIDATE(vkCreateCommandPool(device, &poolInfo, nullptr, &returnCommandPool), "failed to create command pool!");

		return returnCommandPool;
	}

	[[nodiscard]] std::vector<std::array<VkCommandBuffer, 3>> allocateCommandBuffers(const MRG::Vulkan::WindowProperties* data)
	{
		std::vector<std::array<VkCommandBuffer, 3>> commandBuffers(data->swapChain.frameBuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = data->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 3;

		for (std::size_t i = 0; i < commandBuffers.size(); ++i)
			MRG_VKVALIDATE(vkAllocateCommandBuffers(data->device, &allocInfo, commandBuffers[i].data()),
			               "failed to allocate command buffers!");
		MRG_ENGINE_TRACE("{} command buffers successfully allocated", commandBuffers.size());

		return commandBuffers;
	}

	[[nodiscard]] MRG::Vulkan::LightVulkanImage createDepthBuffer(MRG::Vulkan::WindowProperties* data)
	{
		MRG::Vulkan::LightVulkanImage depthBuffer;

		const auto depthFormat = findDepthFormat(data->physicalDevice);

		MRG::Vulkan::createImage(data->physicalDevice,
		                         data->device,
		                         data->swapChain.extent.width,
		                         data->swapChain.extent.height,
		                         depthFormat,
		                         VK_IMAGE_TILING_OPTIMAL,
		                         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                         depthBuffer.handle,
		                         depthBuffer.memoryHandle);
		depthBuffer.imageView = MRG::Vulkan::createImageView(data->device, depthBuffer.handle, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		const auto commandBuffer = beginSingleTimeCommand(data);

		VkPipelineStageFlags sourceStage, destinationStage;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = depthBuffer.handle;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		endSingleTimeCommand(data, commandBuffer);

		return depthBuffer;
	}

	std::pair<VkDescriptorPool, std::vector<VkDescriptorSet>> createDescriptorPool(const MRG::Vulkan::WindowProperties* data,
	                                                                               uint32_t textureCount)
	{
		VkDescriptorPool descriptorPool;

		std::array<VkDescriptorPoolSize, 1> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[0].descriptorCount = data->swapChain.imageCount * textureCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = data->swapChain.imageCount;

		MRG_VKVALIDATE(vkCreateDescriptorPool(data->device, &poolInfo, nullptr, &descriptorPool), "failed to create descriptor pool!");

		std::vector<VkDescriptorSet> descriptorSets(data->swapChain.imageCount);

		std::vector<VkDescriptorSetLayout> layout(data->swapChain.imageCount, data->descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = data->swapChain.imageCount;
		allocInfo.pSetLayouts = layout.data();

		MRG_VKVALIDATE(vkAllocateDescriptorSets(data->device, &allocInfo, descriptorSets.data()), "failed to allocate descriptor sets!");

		return {descriptorPool, descriptorSets};
	}

}  // namespace

namespace MRG::Vulkan
{
	void Renderer2D::init()
	{
		MRG_PROFILE_FUNCTION();

		m_data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(MRG::Renderer2D::getGLFWWindow()));
		m_textureShader = createRef<Shader>("resources/shaders/texture");

		m_data->swapChain = createSwapChain(m_data->physicalDevice, m_data->surface, m_data->device, m_data);
		MRG_ENGINE_INFO("Vulkan swap chain successfully created");

		auto [clearingRP, renderingRP, ImGuiRP] = createRenderPasses(m_data->physicalDevice, m_data->device, m_data->swapChain.imageFormat);
		m_data->clearingPipeline.renderPass = clearingRP;
		m_data->renderingPipeline.renderPass = renderingRP;
		m_data->ImGuiPipeline.renderPass = ImGuiRP;

		m_data->commandPool = createCommandPool(m_data->device, m_data->physicalDevice, m_data->surface);
		MRG_ENGINE_TRACE("Command pool successfully created");

		m_vertexArray = createRef<VertexArray>();

		const auto vertexBuffer = createRef<VertexBuffer>(static_cast<uint32_t>(maxVertices * sizeof(QuadVertex)));
		vertexBuffer->layout = QuadVertex::layout;
		m_vertexArray->addVertexBuffer(vertexBuffer);

		m_qvbBase = new QuadVertex[maxVertices];
		uint32_t* quadIndices = new uint32_t[maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < maxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		const auto indexBuffer = std::static_pointer_cast<Vulkan::IndexBuffer>(IndexBuffer::create(quadIndices, maxIndices));
		m_vertexArray->setIndexBuffer(indexBuffer);
		delete[] quadIndices;

		m_whiteTexture = createRef<Texture2D>(1, 1);
		auto whiteTextureData = 0xffffffff;
		m_whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		m_textureSlots[0] = m_whiteTexture;

		m_data->descriptorSetLayout = createDescriptorSetLayout(m_data->device, maxTextureSlots);

		auto [pool, descriptors] = createDescriptorPool(m_data, maxTextureSlots);
		m_descriptorPool = pool;
		m_descriptorSets = descriptors;

		m_data->pushConstantRanges = populatePushConstantsRanges();

		const auto pipelineLayoutCreateInfo = populatePipelineLayout(&m_data->descriptorSetLayout, &m_data->pushConstantRanges);
		VkPipelineLayout layout;
		MRG_VKVALIDATE(vkCreatePipelineLayout(m_data->device, &pipelineLayoutCreateInfo, nullptr, &layout),
		               "failed to create pipeline layout!");
		m_data->clearingPipeline.layout = layout;
		m_data->renderingPipeline.layout = layout;
		m_data->ImGuiPipeline.layout = layout;
		MRG_ENGINE_TRACE("Vulkan graphics pipeline layout successfully created");

		auto [clearingPipeline, renderingPipeline, ImGuiPipeline] =
		  createPipelines(m_data,
		                  m_textureShader,
		                  std::static_pointer_cast<MRG::Vulkan::VertexArray>(m_vertexArray)->getAttributeDescriptions(),
		                  {std::static_pointer_cast<MRG::Vulkan::VertexArray>(m_vertexArray)->getBindingDescription()});
		m_data->clearingPipeline.handle = clearingPipeline;
		m_data->renderingPipeline.handle = renderingPipeline;
		m_data->ImGuiPipeline.handle = ImGuiPipeline;
		MRG_ENGINE_INFO("Vulkan graphics pipeline successfully created");

		m_data->swapChain.depthBuffer = createDepthBuffer(m_data);

		m_data->swapChain.frameBuffers =
		  createFramebuffers(m_data->device,
		                     m_data->swapChain.imageViews,
		                     m_data->swapChain.depthBuffer.imageView,
		                     {m_data->clearingPipeline.renderPass, m_data->renderingPipeline.renderPass, m_data->ImGuiPipeline.renderPass},
		                     m_data->swapChain.extent);
		MRG_ENGINE_TRACE("Framebuffers successfully created");

		m_data->commandBuffers = allocateCommandBuffers(m_data);

		m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
		m_inFlightFences.resize(m_maxFramesInFlight);
		m_imagesInFlight.resize(m_data->swapChain.imageCount, VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (std::size_t i = 0; i < m_maxFramesInFlight; ++i) {
			MRG_VKVALIDATE(vkCreateSemaphore(m_data->device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]),
			               "failed to create semaphores for a frame!");
			MRG_VKVALIDATE(vkCreateFence(m_data->device, &fenceInfo, nullptr, &m_inFlightFences[i]),
			               "failed to create fences for a frame!");
		}
	}

	void Renderer2D::shutdown()
	{
		MRG_PROFILE_FUNCTION();

		vkDeviceWaitIdle(m_data->device);

		for (std::size_t i = 0; i < m_maxFramesInFlight; ++i) {
			vkDestroySemaphore(m_data->device, m_imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_data->device, m_inFlightFences[i], nullptr);
		}

		cleanupSwapChain();

		vkDestroyPipeline(m_data->device, m_data->clearingPipeline.handle, nullptr);
		vkDestroyPipeline(m_data->device, m_data->renderingPipeline.handle, nullptr);
		vkDestroyPipeline(m_data->device, m_data->ImGuiPipeline.handle, nullptr);

		// All 3 pipelines have the same VkLayout handle, so destroying only one is necessary
		vkDestroyPipelineLayout(m_data->device, m_data->clearingPipeline.layout, nullptr);

		vkDestroyDescriptorSetLayout(m_data->device, m_data->descriptorSetLayout, nullptr);

		m_vertexArray->destroy();
		m_textureShader->destroy();

		m_whiteTexture->destroy();
		for (const auto& texture : m_textureSlots) {
			if (texture != nullptr)
				texture->destroy();
		}

		if (m_renderTarget != nullptr) {
			m_renderTarget->destroy();
		}

		vkDestroyCommandPool(m_data->device, m_data->commandPool, nullptr);

		delete[] m_qvbBase;
	}

	void Renderer2D::onWindowResize(uint32_t, uint32_t) { m_shouldRecreateSwapChain = true; }

	bool Renderer2D::beginFrame()
	{
		MRG_PROFILE_FUNCTION();

		// wait for preview frames to be finished (only allow m_maxFramesInFlight)
		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);

		// Acquire an image from the swapchain
		const auto result = vkAcquireNextImageKHR(m_data->device,
		                                          m_data->swapChain.handle,
		                                          UINT64_MAX,
		                                          m_imageAvailableSemaphores[m_data->currentFrame],
		                                          VK_NULL_HANDLE,
		                                          &m_imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return false;
		}

		if (m_imagesInFlight[m_imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(m_data->device, 1, &m_imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
		}
		m_imagesInFlight[m_imageIndex] = m_inFlightFences[m_data->currentFrame];

		transitionImageLayout(
		  m_data, m_data->swapChain.images[m_imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		return true;
	}

	bool Renderer2D::endFrame()
	{
		MRG_PROFILE_FUNCTION();

		// Execute the command buffer with the current image
		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

		VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkSemaphore signalSempahores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_data->commandBuffers[m_imageIndex][2], 0);

		MRG_VKVALIDATE(vkBeginCommandBuffer(m_data->commandBuffers[m_imageIndex][2], &beginInfo),
		               "failed to begin recording command bufer!");

		if (m_renderTarget != nullptr) {
			transitionImageLayout(m_data,
			                      m_renderTarget->getColorAttachment(),
			                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_data->ImGuiPipeline.renderPass;
		renderPassInfo.framebuffer = m_data->swapChain.frameBuffers[m_imageIndex][2];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_data->swapChain.extent;
		renderPassInfo.clearValueCount = 0;

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_data->commandBuffers[m_imageIndex][2];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSempahores;

		vkCmdBeginRenderPass(m_data->commandBuffers[m_imageIndex][2], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(m_data->commandBuffers[m_imageIndex][2], VK_PIPELINE_BIND_POINT_GRAPHICS, m_data->ImGuiPipeline.handle);

		auto& io = ImGui::GetIO();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_data->commandBuffers[m_imageIndex][2]);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			const auto contextBkp = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(contextBkp);
		}

		vkCmdEndRenderPass(m_data->commandBuffers[m_imageIndex][2]);

		if (m_renderTarget != nullptr) {
			transitionImageLayoutInline(m_data->commandBuffers[m_imageIndex][2],
			                            m_renderTarget->getColorAttachment(),
			                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		MRG_VKVALIDATE(vkEndCommandBuffer(m_data->commandBuffers[m_imageIndex][2]), "failed to record command buffer!");

		MRG_VKVALIDATE(vkQueueSubmit(m_data->graphicsQueue.handle, 1, &submitInfo, m_inFlightFences[m_data->currentFrame]),
		               "failed to submit draw command buffer!");

		VkSwapchainKHR swapChains[] = {m_data->swapChain.handle};

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = waitSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_imageIndex;

		const auto result = vkQueuePresentKHR(m_data->presentQueue.handle, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_shouldRecreateSwapChain) {
			recreateSwapChain();
			m_shouldRecreateSwapChain = false;
		}

		m_data->currentFrame = (m_data->currentFrame + 1) % m_maxFramesInFlight;

		return true;
	}

	void Renderer2D::beginScene(const OrthoCamera& orthoCamera)
	{
		MRG_PROFILE_FUNCTION();

		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

		m_sceneInProgress = true;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_data->commandBuffers[m_imageIndex][1], 0);

		MRG_VKVALIDATE(vkBeginCommandBuffer(m_data->commandBuffers[m_imageIndex][1], &beginInfo),
		               "failed to begin recording command bufer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_data->renderingPipeline.renderPass;
		renderPassInfo.framebuffer =
		  m_renderTarget == nullptr ? m_data->swapChain.frameBuffers[m_imageIndex][1] : m_renderTarget->getHandle();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent =
		  m_renderTarget == nullptr ? m_data->swapChain.extent
		                            : VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		renderPassInfo.clearValueCount = 0;

		vkCmdBeginRenderPass(m_data->commandBuffers[m_imageIndex][1], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width =
		  m_renderTarget == nullptr ? static_cast<float>(m_data->swapChain.extent.width) : m_renderTarget->getSpecification().width;
		viewport.height =
		  m_renderTarget == nullptr ? static_cast<float>(m_data->swapChain.extent.height) : m_renderTarget->getSpecification().height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(m_data->commandBuffers[m_imageIndex][1], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = m_renderTarget == nullptr
		                   ? m_data->swapChain.extent
		                   : VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		vkCmdSetScissor(m_data->commandBuffers[m_imageIndex][1], 0, 1, &scissor);

		vkCmdBindPipeline(m_data->commandBuffers[m_imageIndex][1], VK_PIPELINE_BIND_POINT_GRAPHICS, m_data->renderingPipeline.handle);

		VkBuffer vertexBuffers[] = {std::static_pointer_cast<MRG::Vulkan::VertexBuffer>(m_vertexArray->getVertexBuffers()[0])->getHandle()};
		VkDeviceSize offsets[] = {0};
		auto indexBuffer = std::static_pointer_cast<MRG::Vulkan::IndexBuffer>(m_vertexArray->getIndexBuffer());
		vkCmdBindVertexBuffers(m_data->commandBuffers[m_imageIndex][1], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(m_data->commandBuffers[m_imageIndex][1], indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

		m_modelMatrix.viewProjection = orthoCamera.getProjectionViewMatrix();

		m_quadIndexCount = 0;
		m_qvbPtr = m_qvbBase;

		m_textureSlotindex = 1;
	}

	void Renderer2D::endScene()
	{
		MRG_PROFILE_FUNCTION();

		VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkSemaphore signalSempahores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_data->commandBuffers[m_imageIndex][1];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSempahores;

		if (m_quadIndexCount == 0) {
			vkCmdEndRenderPass(m_data->commandBuffers[m_imageIndex][1]);

			MRG_VKVALIDATE(vkEndCommandBuffer(m_data->commandBuffers[m_imageIndex][1]), "failed to record command buffer!");
			MRG_VKVALIDATE(vkQueueSubmit(m_data->graphicsQueue.handle, 1, &submitInfo, m_inFlightFences[m_data->currentFrame]),
			               "failed to submit draw command buffer!");
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>((uint8_t*)m_qvbPtr - (uint8_t*)m_qvbBase);
		m_vertexArray->getVertexBuffers()[0]->setData(m_qvbBase, dataSize);

		updateDescriptor();

		vkCmdPushConstants(m_data->commandBuffers[m_imageIndex][1],
		                   m_data->renderingPipeline.layout,
		                   VK_SHADER_STAGE_VERTEX_BIT,
		                   0,
		                   sizeof(PushConstants),
		                   &m_modelMatrix);

		vkCmdBindDescriptorSets(m_data->commandBuffers[m_imageIndex][1],
		                        VK_PIPELINE_BIND_POINT_GRAPHICS,
		                        m_data->renderingPipeline.layout,
		                        0,
		                        1,
		                        &m_descriptorSets[m_imageIndex],
		                        0,
		                        nullptr);

		vkCmdDrawIndexed(m_data->commandBuffers[m_imageIndex][1], m_quadIndexCount, 1, 0, 0, 0);
		++m_stats.drawCalls;

		vkCmdEndRenderPass(m_data->commandBuffers[m_imageIndex][1]);

		MRG_VKVALIDATE(vkEndCommandBuffer(m_data->commandBuffers[m_imageIndex][1]), "failed to record command buffer!");

		MRG_VKVALIDATE(vkQueueSubmit(m_data->graphicsQueue.handle, 1, &submitInfo, m_inFlightFences[m_data->currentFrame]),
		               "failed to submit draw command buffer!");

		m_sceneInProgress = false;
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = color;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::drawQuad(
	  const glm::vec3& position, const glm::vec2& size, const Ref<MRG::Texture2D>& texture, float tilingFactor, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		float texIndex = 0.f;
		for (uint32_t i = 0; i < m_textureSlotindex; ++i) {
			if (*m_textureSlots[i].get() == *texture.get()) {
				texIndex = static_cast<float>(i);
				break;
			}
		}

		if (texIndex == 0.f) {
			if (m_textureSlotindex >= maxTextureSlots)
				flushAndReset();

			texIndex = static_cast<float>(m_textureSlotindex);
			m_textureSlots[m_textureSlotindex] = texture;
			++m_textureSlotindex;
		}

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = color;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		const float texIndex = 0.0f;
		const float tilingFactor = 1.0f;

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f}) *
		                 glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = color;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position,
	                                 const glm::vec2& size,
	                                 float rotation,
	                                 const Ref<MRG::Texture2D>& texture,
	                                 float tilingFactor,
	                                 const glm::vec4& color)
	{
		MRG_PROFILE_FUNCTION();

		if (m_quadIndexCount >= maxIndices)
			flushAndReset();

		float texIndex = 0.f;
		for (uint32_t i = 0; i < m_textureSlotindex; ++i) {
			if (*m_textureSlots[i].get() == *texture.get()) {
				texIndex = static_cast<float>(i);
				break;
			}
		}

		if (texIndex == 0.f) {
			if (m_textureSlotindex >= maxTextureSlots)
				flushAndReset();

			texIndex = static_cast<float>(m_textureSlotindex);
			m_textureSlots[m_textureSlotindex] = texture;
			++m_textureSlotindex;
		}

		auto transform = glm::translate(glm::mat4{1.f}, position) * glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f}) *
		                 glm::rotate(glm::mat4{1.f}, rotation, {0.f, 0.f, 1.f});

		for (std::size_t i = 0; i < m_quadVertexCount; ++i) {
			m_qvbPtr->position = transform * m_quadVertexPositions[i];
			m_qvbPtr->color = color;
			m_qvbPtr->texCoord = m_textureCoordinates[i];
			m_qvbPtr->texIndex = texIndex;
			m_qvbPtr->tilingFactor = tilingFactor;
			++m_qvbPtr;
		}

		m_quadIndexCount += 6;
		++m_stats.quadCount;
	}

	void Renderer2D::setRenderTarget(Ref<MRG::Framebuffer> renderTarget)
	{
		MRG_PROFILE_FUNCTION();

		if (renderTarget == nullptr) {
			resetRenderTarget();

			return;
		}

		if (!m_sceneInProgress) {
			m_renderTarget = std::static_pointer_cast<Framebuffer>(renderTarget);

			return;
		}

		endScene();

		m_renderTarget = std::static_pointer_cast<Framebuffer>(renderTarget);

		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_data->commandBuffers[m_imageIndex][1], 0);

		MRG_VKVALIDATE(vkBeginCommandBuffer(m_data->commandBuffers[m_imageIndex][1], &beginInfo),
		               "failed to begin recording command bufer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_data->renderingPipeline.renderPass;
		renderPassInfo.framebuffer = m_renderTarget->getHandle();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		renderPassInfo.clearValueCount = 0;

		vkCmdBeginRenderPass(m_data->commandBuffers[m_imageIndex][1], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(m_renderTarget->getSpecification().width);
		viewport.height = static_cast<float>(m_renderTarget->getSpecification().height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(m_data->commandBuffers[m_imageIndex][1], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		vkCmdSetScissor(m_data->commandBuffers[m_imageIndex][1], 0, 1, &scissor);

		vkCmdBindPipeline(m_data->commandBuffers[m_imageIndex][1], VK_PIPELINE_BIND_POINT_GRAPHICS, m_data->renderingPipeline.handle);

		VkBuffer vertexBuffers[] = {std::static_pointer_cast<MRG::Vulkan::VertexBuffer>(m_vertexArray->getVertexBuffers()[0])->getHandle()};
		VkDeviceSize offsets[] = {0};
		auto indexBuffer = std::static_pointer_cast<MRG::Vulkan::IndexBuffer>(m_vertexArray->getIndexBuffer());
		vkCmdBindVertexBuffers(m_data->commandBuffers[m_imageIndex][1], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(m_data->commandBuffers[m_imageIndex][1], indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

		m_quadIndexCount = 0;
		m_qvbPtr = m_qvbBase;

		m_textureSlotindex = 1;

		m_sceneInProgress = true;
	}

	void Renderer2D::resetRenderTarget()
	{
		MRG_PROFILE_FUNCTION();

		if (!m_sceneInProgress) {
			m_renderTarget = nullptr;
			return;
		}

		endScene();

		m_renderTarget = nullptr;

		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_data->commandBuffers[m_imageIndex][1], 0);

		MRG_VKVALIDATE(vkBeginCommandBuffer(m_data->commandBuffers[m_imageIndex][1], &beginInfo),
		               "failed to begin recording command bufer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_data->renderingPipeline.renderPass;
		renderPassInfo.framebuffer = m_data->swapChain.frameBuffers[m_imageIndex][1];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_data->swapChain.extent;
		renderPassInfo.clearValueCount = 0;

		vkCmdBeginRenderPass(m_data->commandBuffers[m_imageIndex][1], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float>(m_data->swapChain.extent.width);
		viewport.height = static_cast<float>(m_data->swapChain.extent.height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(m_data->commandBuffers[m_imageIndex][1], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = m_data->swapChain.extent;
		vkCmdSetScissor(m_data->commandBuffers[m_imageIndex][1], 0, 1, &scissor);

		vkCmdBindPipeline(m_data->commandBuffers[m_imageIndex][1], VK_PIPELINE_BIND_POINT_GRAPHICS, m_data->renderingPipeline.handle);

		VkBuffer vertexBuffers[] = {std::static_pointer_cast<MRG::Vulkan::VertexBuffer>(m_vertexArray->getVertexBuffers()[0])->getHandle()};
		VkDeviceSize offsets[] = {0};
		auto indexBuffer = std::static_pointer_cast<MRG::Vulkan::IndexBuffer>(m_vertexArray->getIndexBuffer());
		vkCmdBindVertexBuffers(m_data->commandBuffers[m_imageIndex][1], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(m_data->commandBuffers[m_imageIndex][1], indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

		m_quadIndexCount = 0;
		m_qvbPtr = m_qvbBase;

		m_textureSlotindex = 1;

		m_sceneInProgress = true;
	}

	void Renderer2D::clear()
	{
		MRG_PROFILE_FUNCTION();

		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_data->commandBuffers[m_imageIndex][0], 0);

		MRG_VKVALIDATE(vkBeginCommandBuffer(m_data->commandBuffers[m_imageIndex][0], &beginInfo),
		               "failed to begin recording command bufer!");

		std::array<VkClearValue, 2> clearColors{};
		clearColors[0].color = {{m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a}};
		clearColors[1].depthStencil = {1.f, 0};

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_data->clearingPipeline.renderPass;
		renderPassInfo.framebuffer =
		  m_renderTarget == nullptr ? m_data->swapChain.frameBuffers[m_imageIndex][0] : m_renderTarget->getHandle();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent =
		  m_renderTarget == nullptr ? m_data->swapChain.extent
		                            : VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearColors.size());
		renderPassInfo.pClearValues = clearColors.data();

		VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkSemaphore signalSempahores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_data->commandBuffers[m_imageIndex][0];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSempahores;

		vkCmdBeginRenderPass(m_data->commandBuffers[m_imageIndex][0], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width =
		  m_renderTarget == nullptr ? static_cast<float>(m_data->swapChain.extent.width) : m_renderTarget->getSpecification().width;
		viewport.height =
		  m_renderTarget == nullptr ? static_cast<float>(m_data->swapChain.extent.height) : m_renderTarget->getSpecification().height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(m_data->commandBuffers[m_imageIndex][0], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = m_renderTarget == nullptr
		                   ? m_data->swapChain.extent
		                   : VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		vkCmdSetScissor(m_data->commandBuffers[m_imageIndex][0], 0, 1, &scissor);

		vkCmdBindPipeline(m_data->commandBuffers[m_imageIndex][0], VK_PIPELINE_BIND_POINT_GRAPHICS, m_data->clearingPipeline.handle);

		vkCmdEndRenderPass(m_data->commandBuffers[m_imageIndex][0]);

		MRG_VKVALIDATE(vkEndCommandBuffer(m_data->commandBuffers[m_imageIndex][0]), "failed to record command buffer!");

		MRG_VKVALIDATE(vkQueueSubmit(m_data->graphicsQueue.handle, 1, &submitInfo, m_inFlightFences[m_data->currentFrame]),
		               "failed to submit draw command buffer!");
	}

	void Renderer2D::cleanupSwapChain()
	{
		MRG_PROFILE_FUNCTION();

		vkDestroyImageView(m_data->device, m_data->swapChain.depthBuffer.imageView, nullptr);
		vkDestroyImage(m_data->device, m_data->swapChain.depthBuffer.handle, nullptr);
		vkFreeMemory(m_data->device, m_data->swapChain.depthBuffer.memoryHandle, nullptr);

		for (auto framebuffers : m_data->swapChain.frameBuffers) {
			for (auto framebuffer : framebuffers) vkDestroyFramebuffer(m_data->device, framebuffer, nullptr);
		}

		for (std::size_t i = 0; i < m_data->commandBuffers.size(); ++i)
			vkFreeCommandBuffers(m_data->device, m_data->commandPool, 3, m_data->commandBuffers[i].data());

		vkDestroyRenderPass(m_data->device, m_data->clearingPipeline.renderPass, nullptr);
		vkDestroyRenderPass(m_data->device, m_data->renderingPipeline.renderPass, nullptr);
		vkDestroyRenderPass(m_data->device, m_data->ImGuiPipeline.renderPass, nullptr);

		for (auto imageView : m_data->swapChain.imageViews) vkDestroyImageView(m_data->device, imageView, nullptr);

		vkDestroySwapchainKHR(m_data->device, m_data->swapChain.handle, nullptr);

		vkDestroyDescriptorPool(m_data->device, m_descriptorPool, nullptr);
	}

	void Renderer2D::recreateSwapChain()
	{
		MRG_PROFILE_FUNCTION();

		int width = m_data->width, height = m_data->height;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(MRG::Renderer2D::getGLFWWindow(), &width, &height);
			glfwWaitEvents();
		}

		m_data->width = width;
		m_data->height = height;

		MRG_ENGINE_TRACE("Recreating swap chain");
		MRG_ENGINE_TRACE("Waiting for device to be idle...");
		vkDeviceWaitIdle(m_data->device);

		cleanupSwapChain();

		m_data->swapChain = createSwapChain(m_data->physicalDevice, m_data->surface, m_data->device, m_data);
		MRG_ENGINE_INFO("Vulkan swap chain succesfully recreated");

		auto [clearingRP, renderingRP, ImGuiRP] = createRenderPasses(m_data->physicalDevice, m_data->device, m_data->swapChain.imageFormat);
		m_data->clearingPipeline.renderPass = clearingRP;
		m_data->renderingPipeline.renderPass = renderingRP;
		m_data->ImGuiPipeline.renderPass = ImGuiRP;

		m_data->swapChain.depthBuffer = createDepthBuffer(m_data);

		MRG_ENGINE_INFO("Vulkan graphics pipeline successfully created");
		m_data->swapChain.frameBuffers =
		  createFramebuffers(m_data->device,
		                     m_data->swapChain.imageViews,
		                     m_data->swapChain.depthBuffer.imageView,
		                     {m_data->clearingPipeline.renderPass, m_data->renderingPipeline.renderPass, m_data->ImGuiPipeline.renderPass},
		                     m_data->swapChain.extent);
		MRG_ENGINE_TRACE("Framebuffers successfully created");

		auto [pool, descriptors] = createDescriptorPool(m_data, maxTextureSlots);
		m_descriptorPool = pool;
		m_descriptorSets = descriptors;

		m_data->commandBuffers = allocateCommandBuffers(m_data);

		if (m_renderTarget != nullptr) {
			m_renderTarget->invalidate();
		}
	}

	void Renderer2D::updateDescriptor()
	{
		MRG_PROFILE_FUNCTION();

		VkDescriptorImageInfo imageInfos[maxTextureSlots]{};
		for (uint32_t i = 0; i < maxTextureSlots; ++i) {
			imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfos[i].imageView = (i < m_textureSlotindex)
			                            ? std::static_pointer_cast<Vulkan::Texture2D>(m_textureSlots[i])->getImageView()
			                            : std::static_pointer_cast<Vulkan::Texture2D>(m_textureSlots[0])->getImageView();
			imageInfos[i].sampler = (i < m_textureSlotindex) ? std::static_pointer_cast<Vulkan::Texture2D>(m_textureSlots[i])->getSampler()
			                                                 : std::static_pointer_cast<Vulkan::Texture2D>(m_textureSlots[0])->getSampler();
		}

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptorSets[m_imageIndex];
		descriptorWrites[0].dstBinding = 1;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = maxTextureSlots;
		descriptorWrites[0].pImageInfo = imageInfos;

		vkUpdateDescriptorSets(m_data->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void Renderer2D::flushAndReset()
	{
		MRG_PROFILE_FUNCTION();

		endScene();

		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);
		vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkResetCommandBuffer(m_data->commandBuffers[m_imageIndex][1], 0);

		MRG_VKVALIDATE(vkBeginCommandBuffer(m_data->commandBuffers[m_imageIndex][1], &beginInfo),
		               "failed to begin recording command bufer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_data->renderingPipeline.renderPass;
		renderPassInfo.framebuffer =
		  m_renderTarget == nullptr ? m_data->swapChain.frameBuffers[m_imageIndex][1] : m_renderTarget->getHandle();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent =
		  m_renderTarget == nullptr ? m_data->swapChain.extent
		                            : VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		renderPassInfo.clearValueCount = 0;

		vkCmdBeginRenderPass(m_data->commandBuffers[m_imageIndex][1], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width =
		  m_renderTarget == nullptr ? static_cast<float>(m_data->swapChain.extent.width) : m_renderTarget->getSpecification().width;
		viewport.height =
		  m_renderTarget == nullptr ? static_cast<float>(m_data->swapChain.extent.height) : m_renderTarget->getSpecification().height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(m_data->commandBuffers[m_imageIndex][1], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = m_renderTarget == nullptr
		                   ? m_data->swapChain.extent
		                   : VkExtent2D{m_renderTarget->getSpecification().width, m_renderTarget->getSpecification().height};
		vkCmdSetScissor(m_data->commandBuffers[m_imageIndex][1], 0, 1, &scissor);

		vkCmdBindPipeline(m_data->commandBuffers[m_imageIndex][1], VK_PIPELINE_BIND_POINT_GRAPHICS, m_data->renderingPipeline.handle);

		VkBuffer vertexBuffers[] = {std::static_pointer_cast<MRG::Vulkan::VertexBuffer>(m_vertexArray->getVertexBuffers()[0])->getHandle()};
		VkDeviceSize offsets[] = {0};
		auto indexBuffer = std::static_pointer_cast<MRG::Vulkan::IndexBuffer>(m_vertexArray->getIndexBuffer());
		vkCmdBindVertexBuffers(m_data->commandBuffers[m_imageIndex][1], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(m_data->commandBuffers[m_imageIndex][1], indexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

		m_quadIndexCount = 0;
		m_qvbPtr = m_qvbBase;

		m_textureSlotindex = 1;
	}
}  // namespace MRG::Vulkan
