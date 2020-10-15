#include "Renderer2D.h"

#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/Renderer2D.h"

#include <array>

namespace
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

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
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = imageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			MRG_VKVALIDATE(vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]), "failed to create image views!");
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
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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

		return {handle, minImageCount, imageCount, images, surfaceFormat.format, extent, imageViews, {}};
	}

	[[nodiscard]] VkPipelineLayoutCreateInfo populatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		return pipelineCreateInfo;
	}

	[[nodiscard]] VkRenderPass createRenderPass(VkDevice device, VkFormat swapChainFormat)
	{
		VkRenderPass returnRenderPass;

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		MRG_VKVALIDATE(vkCreateRenderPass(device, &renderPassInfo, nullptr, &returnRenderPass), "failed to create render pass!")
		return returnRenderPass;
	}

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

		const auto bindingDescription = Vertex::getBindingDescription();
		const auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

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

	[[nodiscard]] VkCommandPool createCommandPool(const VkDevice device, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface)
	{
		VkCommandPool returnCommandPool;

		const auto queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		MRG_VKVALIDATE(vkCreateCommandPool(device, &poolInfo, nullptr, &returnCommandPool), "failed to create command pool!");

		return returnCommandPool;
	}

	[[nodiscard]] uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
			if ((typeFilter & MRG_BIT(i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	[[nodiscard]] std::pair<VkBuffer, VkDeviceMemory>
	createVertexBuffer(const VkPhysicalDevice physicalDevice, VkDevice device, std::size_t bufferSize, const void* bufferData)
	{
		VkBuffer returnBuffer;
		VkDeviceMemory returnMemory;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		MRG_VKVALIDATE(vkCreateBuffer(device, &bufferInfo, nullptr, &returnBuffer), "failed to create vertex buffer!");
		MRG_ENGINE_TRACE("Vertex buffer succesfully created");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, returnBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(
		  physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		MRG_VKVALIDATE(vkAllocateMemory(device, &allocInfo, nullptr, &returnMemory), "failed to allocate vertex buffer memory");
		MRG_ENGINE_TRACE("Vertex buffer memory successfully allocated");

		vkBindBufferMemory(device, returnBuffer, returnMemory, 0);

		void* data;
		vkMapMemory(device, returnMemory, 0, bufferSize, 0, &data);
		memcpy(data, bufferData, bufferSize);
		vkUnmapMemory(device, returnMemory);
		MRG_ENGINE_TRACE("Vertex buffer data successfully bound and updloaded");

		return {returnBuffer, returnMemory};
	}

	[[nodiscard]] std::vector<VkCommandBuffer>
	createCommandBuffers(const MRG::Vulkan::WindowProperties* data, VkBuffer vertexBuffer, uint32_t vertexCount)
	{
		std::vector<VkCommandBuffer> commandBuffers(data->swapChain.frameBuffers.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = data->commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		MRG_VKVALIDATE(vkAllocateCommandBuffers(data->device, &allocInfo, commandBuffers.data()), "failed to allocate command buffers!");
		MRG_ENGINE_TRACE("{} command buffers successfully allocated", commandBuffers.size());

		for (std::size_t i = 0; i < commandBuffers.size(); ++i) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			MRG_VKVALIDATE(vkBeginCommandBuffer(commandBuffers[i], &beginInfo), "failed to begin recording command bufer!");
			MRG_ENGINE_TRACE("Beginning recording command buffer {}", i);

			VkClearValue clearColor = {{{.0f, .0f, .0f, 1.f}}};
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = data->pipeline.renderPass;
			renderPassInfo.framebuffer = data->swapChain.frameBuffers[i];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = data->swapChain.extent;
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			MRG_ENGINE_TRACE("\tRender pass started");

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, data->pipeline.handle);
			MRG_ENGINE_TRACE("\t\tGraphics pipeline bound");

			VkBuffer vertexBuffers[] = {vertexBuffer};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			MRG_ENGINE_TRACE("\t\tVertex buffer bound");

			vkCmdDraw(commandBuffers[i], vertexCount, 1, 0, 0);
			MRG_ENGINE_TRACE("\t\tDraw call recorded");

			vkCmdEndRenderPass(commandBuffers[i]);
			MRG_ENGINE_TRACE("\tRender pass ended");

			MRG_VKVALIDATE(vkEndCommandBuffer(commandBuffers[i]), "failed to record command buffer!");
			MRG_ENGINE_TRACE("Command buffer {} successfully recorded", i);
		}

		return commandBuffers;
	}

	void cleanupSwapChain(MRG::Vulkan::WindowProperties* data)
	{
		for (auto framebuffer : data->swapChain.frameBuffers) vkDestroyFramebuffer(data->device, framebuffer, nullptr);

		vkFreeCommandBuffers(
		  data->device, data->commandPool, static_cast<uint32_t>(data->commandBuffers.size()), data->commandBuffers.data());

		vkDestroyPipeline(data->device, data->pipeline.handle, nullptr);

		vkDestroyPipelineLayout(data->device, data->pipeline.layout, nullptr);

		vkDestroyRenderPass(data->device, data->pipeline.renderPass, nullptr);

		for (auto imageView : data->swapChain.imageViews) vkDestroyImageView(data->device, imageView, nullptr);

		vkDestroySwapchainKHR(data->device, data->swapChain.handle, nullptr);
	}

	void recreateSwapChain(MRG::Vulkan::WindowProperties* data,
	                       MRG::Ref<MRG::Vulkan::Shader> textureShader,
	                       VkBuffer vertexBuffer,
	                       uint32_t vertexCount)
	{
		int width = data->width, height = data->height;
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(MRG::Renderer2D::getGLFWWindow(), &width, &height);
			glfwWaitEvents();
		}

		data->width = width;
		data->height = height;

		MRG_ENGINE_TRACE("Recreating swap chain");
		MRG_ENGINE_TRACE("Waiting for device to be idle...");
		vkDeviceWaitIdle(data->device);

		cleanupSwapChain(data);

		data->swapChain = createSwapChain(data->physicalDevice, data->surface, data->device, data);
		MRG_ENGINE_INFO("Vulkan swap chain succesfully recreated");

		data->pipeline.renderPass = createRenderPass(data->device, data->swapChain.imageFormat);

		const auto pipelineLayoutCreateInfo = populatePipelineLayout();
		MRG_VKVALIDATE(vkCreatePipelineLayout(data->device, &pipelineLayoutCreateInfo, nullptr, &data->pipeline.layout),
		               "failed to recreate pipeline layout!");
		MRG_ENGINE_TRACE("Vulkan graphics pipeline layout successfully created");
		data->pipeline.handle = createPipeline(data, textureShader);

		MRG_ENGINE_INFO("Vulkan graphics pipeline successfully created");
		data->swapChain.frameBuffers =
		  createframeBuffers(data->device, data->swapChain.imageViews, data->pipeline.renderPass, data->swapChain.extent);
		MRG_ENGINE_TRACE("Framebuffers successfully created");

		data->commandBuffers = createCommandBuffers(data, vertexBuffer, vertexCount);
	}

}  // namespace

namespace MRG::Vulkan
{
	void Renderer2D::init()
	{
		MRG_PROFILE_FUNCTION();

		m_data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(MRG::Renderer2D::getGLFWWindow()));
		m_textureShader = createRef<Shader>("resources/shaders/texture");

		const std::vector<Vertex> vertices = {
		  {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

		try {
			m_data->swapChain = createSwapChain(m_data->physicalDevice, m_data->surface, m_data->device, m_data);
			MRG_ENGINE_INFO("Vulkan swap chain successfully created");

			m_data->pipeline.renderPass = createRenderPass(m_data->device, m_data->swapChain.imageFormat);

			const auto pipelineLayoutCreateInfo = populatePipelineLayout();
			MRG_VKVALIDATE(vkCreatePipelineLayout(m_data->device, &pipelineLayoutCreateInfo, nullptr, &m_data->pipeline.layout),
			               "failed to create pipeline layout!");
			MRG_ENGINE_TRACE("Vulkan graphics pipeline layout successfully created");

			m_data->pipeline.handle = createPipeline(m_data, m_textureShader);
			MRG_ENGINE_INFO("Vulkan graphics pipeline successfully created");

			m_data->swapChain.frameBuffers =
			  createframeBuffers(m_data->device, m_data->swapChain.imageViews, m_data->pipeline.renderPass, m_data->swapChain.extent);
			MRG_ENGINE_TRACE("Framebuffers successfully created");

			m_data->commandPool = createCommandPool(m_data->device, m_data->physicalDevice, m_data->surface);
			MRG_ENGINE_TRACE("Command pool successfully created");

			const auto [vertexBuffer, vertexBufferMemory] = createVertexBuffer(
			  m_data->physicalDevice, m_data->device, sizeof(Vertex) * vertices.size(), static_cast<const void*>(vertices.data()));
			m_vertexBuffer = vertexBuffer;
			m_vertexBufferMemory = vertexBufferMemory;

			m_data->commandBuffers = createCommandBuffers(m_data, m_vertexBuffer, static_cast<uint32_t>(vertices.size()));

			m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
			m_renderFinishedSemaphores.resize(m_maxFramesInFlight);
			m_inFlightFences.resize(m_maxFramesInFlight);
			m_imagesInFlight.resize(m_data->swapChain.images.size(), VK_NULL_HANDLE);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (std::size_t i = 0; i < m_maxFramesInFlight; ++i) {
				MRG_VKVALIDATE(vkCreateSemaphore(m_data->device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]),
				               "failed to create semaphores for a frame!");
				MRG_VKVALIDATE(vkCreateSemaphore(m_data->device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]),
				               "failed to create semaphores for a frame!");
				MRG_VKVALIDATE(vkCreateFence(m_data->device, &fenceInfo, nullptr, &m_inFlightFences[i]),
				               "failed to create fences for a frame!");
			}
		} catch (const std::runtime_error& e) {
			MRG_ENGINE_ERROR("Vulkan error detected: {}", e.what());
		}
	}

	void Renderer2D::shutdown()
	{
		MRG_PROFILE_FUNCTION();

		vkDeviceWaitIdle(m_data->device);

		for (std::size_t i = 0; i < m_maxFramesInFlight; ++i) {
			vkDestroySemaphore(m_data->device, m_imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(m_data->device, m_renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(m_data->device, m_inFlightFences[i], nullptr);
		}

		cleanupSwapChain(m_data);

		vkDestroyBuffer(m_data->device, m_vertexBuffer, nullptr);
		vkFreeMemory(m_data->device, m_vertexBufferMemory, nullptr);

		vkDestroyCommandPool(m_data->device, m_data->commandPool, nullptr);

		m_textureShader->destroy();
	}

	void Renderer2D::onWindowResize(uint32_t, uint32_t) { m_shouldRecreateSwapChain = true; }

	bool Renderer2D::beginFrame()
	{
		// wait for preview frames to be finished (only allow m_maxFramesInFlight)
		vkWaitForFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame], VK_TRUE, UINT64_MAX);

		// Acquire an image from the swapchain
		try {
			const auto result = vkAcquireNextImageKHR(m_data->device,
			                                          m_data->swapChain.handle,
			                                          UINT64_MAX,
			                                          m_imageAvailableSemaphores[m_data->currentFrame],
			                                          VK_NULL_HANDLE,
			                                          &m_imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				recreateSwapChain(m_data, m_textureShader, m_vertexBuffer, 3);
				return false;
			}

			if (m_imagesInFlight[m_imageIndex] != VK_NULL_HANDLE) {
				vkWaitForFences(m_data->device, 1, &m_imagesInFlight[m_imageIndex], VK_TRUE, UINT64_MAX);
			}
			m_imagesInFlight[m_imageIndex] = m_inFlightFences[m_data->currentFrame];
		} catch (const std::runtime_error& e) {
			MRG_ENGINE_ERROR("Vulkan error detected: {}", e.what());
		}
		return true;
	}

	bool Renderer2D::endFrame()
	{
		// TODO: Return the image for swapchain presentation
		VkSemaphore signalSempahores[] = {m_renderFinishedSemaphores[m_data->currentFrame]};
		VkSwapchainKHR swapChains[] = {m_data->swapChain.handle};

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSempahores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_imageIndex;

		const auto result = vkQueuePresentKHR(m_data->presentQueue.handle, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_shouldRecreateSwapChain) {
			recreateSwapChain(m_data, m_textureShader, m_vertexBuffer, 3);
			m_shouldRecreateSwapChain = false;
		}

		m_data->currentFrame = (m_data->currentFrame + 1) % m_maxFramesInFlight;

		return true;
	}

	void Renderer2D::beginScene(const OrthoCamera&) {}
	void Renderer2D::endScene() {}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const glm::vec4&)
	{
		// This function is the draw call to the triangle. I know it's wierd, it's just a placeholder for now, I swear
		// Execute the command buffer with the current image
		VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_data->currentFrame]};
		VkSemaphore signalSempahores[] = {m_renderFinishedSemaphores[m_data->currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_data->commandBuffers[m_imageIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSempahores;

		try {
			vkResetFences(m_data->device, 1, &m_inFlightFences[m_data->currentFrame]);

			MRG_VKVALIDATE(vkQueueSubmit(m_data->graphicsQueue.handle, 1, &submitInfo, m_inFlightFences[m_data->currentFrame]),
			               "failed to submit draw command buffer!");
		} catch (const std::runtime_error& e) {
			MRG_ENGINE_ERROR("Vulkan error detected: {}", e.what());
		}
	}

	void Renderer2D::drawQuad(const glm::vec3&, const glm::vec2&, const Ref<Texture2D>&, float, const glm::vec4&) {}

	void Renderer2D::drawRotatedQuad(const glm::vec3&, const glm::vec2&, float, const glm::vec4&) {}

	void Renderer2D::drawRotatedQuad(const glm::vec3&, const glm::vec2&, float, const Ref<Texture2D>&, float, const glm::vec4&) {}
}  // namespace MRG::Vulkan
