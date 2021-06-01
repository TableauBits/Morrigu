//
// Created by Mathis Lamidey on 2021-04-11.
//

#include "VkRenderer.h"

#include <VkBootstrap.h>

DISABLE_WARNING_PUSH
DISABLE_WARNING_ALIGNMENT_MODIFIED
#include <filesystem>
DISABLE_WARNING_POP
#include <fstream>

namespace
{
	VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                               VkDebugUtilsMessageTypeFlagsEXT,
	                                               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	                                               void*)
	{
		switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
			MRG_ENGINE_TRACE("[VK] {}", pCallbackData->pMessage)
		} break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
			MRG_ENGINE_WARN("[VK] {}", pCallbackData->pMessage)
		} break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
			MRG_ENGINE_ERROR("[VK] {}", pCallbackData->pMessage)
		} break;
		default: {
			MRG_ENGINE_FATAL("[VK] {}", pCallbackData->pMessage)
		} break;
		}

		return VK_FALSE;
	}
}  // namespace

namespace MRG
{
	Ref<Shader> VkRenderer::createShader(const char* vertexShaderName, const char* fragmentShaderName)
	{
		return MRG::createRef<Shader>(m_device, vertexShaderName, fragmentShaderName, m_deletionQueue);
	}

	Ref<Texture> VkRenderer::createTexture(const char* fileName)
	{
		return createRef<Texture>(m_device, m_graphicsQueue, m_uploadContext, m_allocator, fileName, m_deletionQueue);
	}

	void VkRenderer::init(const RendererSpecification& newSpec, GLFWwindow* newWindow)
	{
		spec   = newSpec;
		window = newWindow;

		initVulkan();
		initSwapchain();
		initCommands();
		initDefaultRenderPass();
		initFramebuffers();
		initSyncSructs();
		initDescriptors();
		initAssets();
		initMaterials();

		isInitalized = true;
	}

	void VkRenderer::beginFrame()
	{
		const auto& frameData = getCurrentFrameData();
		MRG_VK_CHECK_HPP(m_device.waitForFences(frameData.renderFence, VK_TRUE, UINT64_MAX), "failed to wait for render fence!")
		m_device.resetFences(frameData.renderFence);

		m_imageIndex = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, frameData.presentSemaphore).value;

		frameData.commandBuffer.reset();
		vk::CommandBufferBeginInfo beginInfo{
		  .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};
		frameData.commandBuffer.begin(beginInfo);

		vk::ClearValue colorClearValue{};
		colorClearValue.color = {std::array<float, 4>{clearColor.r, clearColor.g, clearColor.b}};
		vk::ClearValue depthClearValue{};
		depthClearValue.depthStencil.depth = 1.f;

		std::array<vk::ClearValue, 2> clearValues{colorClearValue, depthClearValue};

		vk::RenderPassBeginInfo renderPassInfo{
		  .renderPass  = m_renderPass,
		  .framebuffer = m_framebuffers[m_imageIndex],
		  .renderArea =
		    vk::Rect2D{
		      .offset = {0, 0},
		      .extent = {static_cast<uint32_t>(spec.windowWidth), static_cast<uint32_t>(spec.windowHeight)},
		    },
		  .clearValueCount = static_cast<uint32_t>(clearValues.size()),
		  .pClearValues    = clearValues.data(),
		};
		frameData.commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	}

	void VkRenderer::endFrame()
	{
		const auto& frameData = getCurrentFrameData();
		frameData.commandBuffer.endRenderPass();
		frameData.commandBuffer.end();

		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo submitInfo{
		  .waitSemaphoreCount   = 1,
		  .pWaitSemaphores      = &frameData.presentSemaphore,
		  .pWaitDstStageMask    = &waitStage,
		  .commandBufferCount   = 1,
		  .pCommandBuffers      = &frameData.commandBuffer,
		  .signalSemaphoreCount = 1,
		  .pSignalSemaphores    = &frameData.renderSemaphore,
		};
		m_graphicsQueue.submit(submitInfo, frameData.renderFence);

		vk::PresentInfoKHR presentInfo{
		  .waitSemaphoreCount = 1,
		  .pWaitSemaphores    = &frameData.renderSemaphore,
		  .swapchainCount     = 1,
		  .pSwapchains        = &m_swapchain,
		  .pImageIndices      = &m_imageIndex,
		};

		MRG_VK_CHECK_HPP(m_graphicsQueue.presentKHR(presentInfo), "failed to present image to screen!")
		++frameNumber;
	}

	void VkRenderer::cleanup()
	{
		if (!isInitalized) { return; }

		m_device.waitIdle();

		m_deletionQueue.flush();

		destroySwapchain();

		vmaDestroyAllocator(m_allocator);

		m_device.destroy();
		m_instance.destroySurfaceKHR(m_surface);
		vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
		m_instance.destroy();

		glfwDestroyWindow(window);
	}

	void VkRenderer::onResize()
	{
		m_device.waitIdle();

		destroySwapchain();
		initSwapchain();
		initDefaultRenderPass();
		initFramebuffers();
	}

	void VkRenderer::draw(const std::vector<RenderData>& drawables, const Camera& camera)
	{
		const auto& frameData = getCurrentFrameData();

		GPUCameraData cameraData{
		  .viewMatrix           = camera.getView(),
		  .projectionMatrix     = camera.getProjection(),
		  .viewProjectionMatrix = camera.getViewProjection(),
		};
		void* data;
		vmaMapMemory(m_allocator, frameData.cameraBuffer.allocation, &data);
		memcpy(data, &cameraData, sizeof(GPUCameraData));
		vmaUnmapMemory(m_allocator, frameData.cameraBuffer.allocation);

		vk::Pipeline currentMaterial{};
		for (const auto& drawable : drawables) {
			if (!(*drawable.isVisible)) { continue; }
			if (currentMaterial != drawable.materialPipeline) {
				frameData.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, drawable.materialPipeline);
				frameData.commandBuffer.setViewport(0,
				                                    vk::Viewport{
				                                      .x        = 0.f,
				                                      .y        = 0.f,
				                                      .width    = static_cast<float>(spec.windowWidth),
				                                      .height   = static_cast<float>(spec.windowHeight),
				                                      .minDepth = 0.f,
				                                      .maxDepth = 1.f,
				                                    });
				frameData.commandBuffer.setScissor(
				  0,
				  vk::Rect2D{
				    .offset{0, 0},
				    .extent = {static_cast<uint32_t>(spec.windowWidth), static_cast<uint32_t>(spec.windowHeight)},
				  });
				frameData.commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
				                                           drawable.materialPipelineLayout,
				                                           0,
				                                           {frameData.level0Descriptor, m_level1Descriptor, drawable.level2Descriptor},
				                                           {});

				currentMaterial = drawable.materialPipeline;
			}

			const auto pushConstants = MeshPushConstants{
			  .modelMatrix = *drawable.modelMatrix,
			};
			frameData.commandBuffer.pushConstants(
			  drawable.materialPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshPushConstants), (void*)(&pushConstants));

			frameData.commandBuffer.bindVertexBuffers(0, drawable.vertexBuffer, {0});
			frameData.commandBuffer.draw(static_cast<uint32_t>(drawable.vertexNumber), 1, 0, 0);
		}
	}

	void VkRenderer::initVulkan()
	{
		// basic instance creation
		vkb::InstanceBuilder instanceBuilder{};
		const auto vkbInstance =
		  instanceBuilder.set_app_name(spec.applicationName.c_str())
		    .require_api_version(1, 1, 0)
#ifdef MRG_DEBUG
		    .request_validation_layers()
		    .set_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		    .set_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
		    .set_debug_callback(vkDebugCallback)
#endif
		    .build()
		    .value();

		m_instance       = vkbInstance.instance;
		m_debugMessenger = vkbInstance.debug_messenger;

		// surface creation
		glfwCreateWindowSurface(m_instance, window, nullptr, (VkSurfaceKHR*)(&m_surface));

		// GPU selection
		vkb::PhysicalDeviceSelector selector{vkbInstance};
		const auto vkbPhysicalDevice = selector.set_minimum_version(1, 1).set_surface(m_surface).select().value();

		m_GPU = vkbPhysicalDevice.physical_device;

		// VkDevice creation
		vkb::DeviceBuilder deviceBuilder{vkbPhysicalDevice};
		const auto vkbDevice = deviceBuilder.build().value();

		m_device             = vkbDevice.device;
		m_graphicsQueue      = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_graphicsQueueIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		// VMA allocator creation
		VmaAllocatorCreateInfo allocatorInfo{
		  .physicalDevice = m_GPU,
		  .device         = m_device,
		  .instance       = m_instance,
		};
		MRG_VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator), "failed to create VMA allocator!")
	}

	void VkRenderer::initSwapchain()
	{
		vkb::SwapchainBuilder swapchainBuilder{m_GPU, m_device, m_surface};
		auto vkbSwapchain = swapchainBuilder.use_default_format_selection()
		                      .set_desired_present_mode(static_cast<VkPresentModeKHR>(spec.preferredPresentMode))
		                      .set_desired_extent(spec.windowWidth, spec.windowHeight)
		                      .build()
		                      .value();

		m_swapchain       = vkbSwapchain.swapchain;
		m_swapchainFormat = vk::Format{vkbSwapchain.image_format};

		const auto rawImages     = vkbSwapchain.get_images().value();
		const auto rawImageViews = vkbSwapchain.get_image_views().value();
		m_swapchainImages        = std::vector<vk::Image>(rawImages.begin(), rawImages.end());
		m_swapchainImageViews    = std::vector<vk::ImageView>(rawImageViews.begin(), rawImageViews.end());

		vk::Extent3D depthImageExtent{
		  .width  = static_cast<uint32_t>(spec.windowWidth),
		  .height = static_cast<uint32_t>(spec.windowHeight),
		  .depth  = 1,
		};
		const auto depthImageCreateInfo =
		  VkInit::imageCreateInfo(m_depthFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, depthImageExtent);
		VmaAllocationCreateInfo depthImageAllocationCreateInfo{.usage         = VMA_MEMORY_USAGE_GPU_ONLY,
		                                                       .requiredFlags = VkMemoryPropertyFlags{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}};

		VkImage rawImage;
		vmaCreateImage(m_allocator, &depthImageCreateInfo, &depthImageAllocationCreateInfo, &rawImage, &m_depthImage.allocation, nullptr);
		m_depthImage.image = rawImage;

		const auto depthImageViewCreateInfo =
		  VkInit::imageViewCreateInfo(m_depthFormat, m_depthImage.image, vk::ImageAspectFlagBits::eDepth);
		m_depthImageView = m_device.createImageView(depthImageViewCreateInfo);
	}

	void VkRenderer::initCommands()
	{
		// create command pool
		const auto cmdPoolInfo = VkInit::cmdPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_graphicsQueueIndex);

		for (auto& frame : m_framesData) {
			frame.commandPool = m_device.createCommandPool(cmdPoolInfo);

			// allocate main command buffer from created command pool
			const auto mainCmdBufferInfo = VkInit::cmdBufferAllocateInfo(frame.commandPool, vk::CommandBufferLevel::ePrimary, 1);
			frame.commandBuffer          = m_device.allocateCommandBuffers(mainCmdBufferInfo)[0];
		}
		m_deletionQueue.push([this]() {
			for (const auto& frame : m_framesData) { m_device.destroyCommandPool(frame.commandPool); }
		});

		// create upload context command pool
		const auto uploadCmdPoolInfo = VkInit::cmdPoolCreateInfo({}, m_graphicsQueueIndex);
		m_uploadContext.commandPool  = m_device.createCommandPool(uploadCmdPoolInfo);
		m_deletionQueue.push([this]() { m_device.destroyCommandPool(m_uploadContext.commandPool); });
	}

	void VkRenderer::initDefaultRenderPass()
	{
		vk::AttachmentDescription colorAttachment{
		  .format         = m_swapchainFormat,
		  .samples        = vk::SampleCountFlagBits::e1,
		  .loadOp         = vk::AttachmentLoadOp::eClear,
		  .storeOp        = vk::AttachmentStoreOp::eStore,
		  .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
		  .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		  .initialLayout  = vk::ImageLayout::eUndefined,
		  .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
		};

		vk::AttachmentReference colorAttachmentRef{
		  .attachment = 0,
		  .layout     = vk::ImageLayout::eColorAttachmentOptimal,
		};

		vk::AttachmentDescription depthAttachment{
		  .format         = m_depthFormat,
		  .samples        = vk::SampleCountFlagBits::e1,
		  .loadOp         = vk::AttachmentLoadOp::eClear,
		  .storeOp        = vk::AttachmentStoreOp::eStore,
		  .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
		  .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		  .initialLayout  = vk::ImageLayout::eUndefined,
		  .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
		};

		vk::AttachmentReference depthAttachmentRef{
		  .attachment = 1,
		  .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
		};

		vk::SubpassDescription subpass{
		  .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
		  .colorAttachmentCount    = 1,
		  .pColorAttachments       = &colorAttachmentRef,
		  .pDepthStencilAttachment = &depthAttachmentRef,
		};

		std::array<vk::AttachmentDescription, 2> attachments{colorAttachment, depthAttachment};

		vk::RenderPassCreateInfo renderPassInfo{
		  .attachmentCount = static_cast<uint32_t>(attachments.size()),
		  .pAttachments    = attachments.data(),
		  .subpassCount    = 1,
		  .pSubpasses      = &subpass,
		};

		m_renderPass = m_device.createRenderPass(renderPassInfo);
	}

	void VkRenderer::initFramebuffers()
	{
		vk::FramebufferCreateInfo framebufferInfo{
		  .renderPass      = m_renderPass,
		  .attachmentCount = 1,
		  .width           = static_cast<uint32_t>(spec.windowWidth),
		  .height          = static_cast<uint32_t>(spec.windowHeight),
		  .layers          = 1,
		};

		const std::size_t swapchainImageCount = m_swapchainImages.size();
		m_framebuffers                        = std::vector<vk::Framebuffer>(swapchainImageCount);

		for (auto i = 0; i < swapchainImageCount; ++i) {
			std::array<vk::ImageView, 2> attachments{m_swapchainImageViews[i], m_depthImageView};
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments    = attachments.data();

			m_framebuffers[i] = m_device.createFramebuffer(framebufferInfo);
		}
	}

	void VkRenderer::initSyncSructs()
	{
		vk::FenceCreateInfo fenceInfo{
		  .flags = vk::FenceCreateFlagBits::eSignaled,
		};

		for (auto& frame : m_framesData) {
			frame.renderFence = m_device.createFence(fenceInfo);
			m_deletionQueue.push([=]() { m_device.destroyFence(frame.renderFence); });

			frame.presentSemaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
			frame.renderSemaphore  = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
			m_deletionQueue.push([=]() {
				m_device.destroySemaphore(frame.presentSemaphore);
				m_device.destroySemaphore(frame.renderSemaphore);
			});
		}

		fenceInfo.flags             = {};
		m_uploadContext.uploadFence = m_device.createFence(fenceInfo);
		m_deletionQueue.push([=]() { m_device.destroyFence(m_uploadContext.uploadFence); });
	}

	void VkRenderer::initDescriptors()
	{
		std::array<vk::DescriptorPoolSize, 1> sizes{{vk::DescriptorType::eUniformBuffer, FRAMES_IN_FLIGHT + 1}};
		vk::DescriptorPoolCreateInfo poolInfo{
		  .maxSets       = FRAMES_IN_FLIGHT + 1,
		  .poolSizeCount = static_cast<uint32_t>(sizes.size()),
		  .pPoolSizes    = sizes.data(),
		};
		m_descriptorPool = m_device.createDescriptorPool(poolInfo);
		m_deletionQueue.push([this]() { m_device.destroyDescriptorPool(m_descriptorPool); });

		vk::DescriptorSetLayoutBinding cameraBufferBinding{
		  .binding         = 0,
		  .descriptorType  = vk::DescriptorType::eUniformBuffer,
		  .descriptorCount = 1,
		  .stageFlags      = vk::ShaderStageFlagBits::eVertex,
		};
		vk::DescriptorSetLayoutCreateInfo setInfo{
		  .bindingCount = 1,
		  .pBindings    = &cameraBufferBinding,
		};
		m_level0DSL = m_device.createDescriptorSetLayout(setInfo);
		m_deletionQueue.push([this]() { m_device.destroyDescriptorSetLayout(m_level0DSL); });

		std::array<vk::DescriptorSetLayout, FRAMES_IN_FLIGHT> layouts{};
		layouts.fill(m_level0DSL);
		vk::DescriptorSetAllocateInfo allocInfo{
		  .descriptorPool     = m_descriptorPool,
		  .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
		  .pSetLayouts        = layouts.data(),
		};
		const auto level0Descriptors = m_device.allocateDescriptorSets(allocInfo);

		vk::DescriptorBufferInfo bufferInfo{
		  .offset = 0,
		  .range  = sizeof(GPUCameraData),
		};
		vk::WriteDescriptorSet setWrite{
		  .dstBinding      = 0,
		  .descriptorCount = 1,
		  .descriptorType  = vk::DescriptorType::eUniformBuffer,
		  .pBufferInfo     = &bufferInfo,
		};

		for (std::size_t i = 0; i < FRAMES_IN_FLIGHT; ++i) {
			m_framesData[i].cameraBuffer = Utils::Allocators::createBuffer(
			  m_allocator, sizeof(GPUCameraData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, m_deletionQueue);
			m_framesData[i].level0Descriptor = level0Descriptors[i];

			bufferInfo.buffer = m_framesData[i].cameraBuffer.buffer;
			setWrite.dstSet   = m_framesData[i].level0Descriptor;
			m_device.updateDescriptorSets(setWrite, {});
		}

		// level 1 DSL (currently empty)
		setInfo.bindingCount = 0;
		m_level1DSL          = m_device.createDescriptorSetLayout(setInfo);
		m_deletionQueue.push([this]() { m_device.destroyDescriptorSetLayout(m_level1DSL); });

		vk::DescriptorSetAllocateInfo allocLeve1Info{
		  .descriptorPool     = m_descriptorPool,
		  .descriptorSetCount = 1,
		  .pSetLayouts        = &m_level1DSL,
		};
		m_level1Descriptor = m_device.allocateDescriptorSets(allocLeve1Info)[0];
	}

	void VkRenderer::initAssets() { defaultTexture = createTexture(Files::Rendering::defaultTexture.c_str()); }

	void VkRenderer::initMaterials()
	{
		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo{};
		if (std::filesystem::exists(Files::Rendering::vkPipelineCacheFile)) {
			MRG_ENGINE_TRACE("Pipeline cache found.")
			std::ifstream pipelineCacheFile{Files::Rendering::vkPipelineCacheFile, std::ios::binary | std::ios::ate};
			const auto fileSize = static_cast<std::size_t>(pipelineCacheFile.tellg());
			std::vector<uint8_t> pipelineData(fileSize / sizeof(uint8_t));
			pipelineCacheFile.seekg(std::ios::beg);
			pipelineCacheFile.read((char*)pipelineData.data(), static_cast<std::streamsize>(fileSize));
			pipelineCacheFile.close();
		}
		m_pipelineCache = m_device.createPipelineCache(pipelineCacheCreateInfo);

		m_deletionQueue.push([=]() {
			const auto newPipelineCacheData = m_device.getPipelineCacheData(m_pipelineCache);
			std::ofstream pipelineCacheFile{Files::Rendering::vkPipelineCacheFile, std::ios::binary | std::ios::trunc};
			pipelineCacheFile.write((char*)newPipelineCacheData.data(), static_cast<std::streamsize>(newPipelineCacheData.size()));
			m_device.destroyPipelineCache(m_pipelineCache);
		});

		defaultBasicShader   = createShader("BasicMesh.vert.spv", "BasicMesh.frag.spv");
		defaultBasicMaterial = createMaterial<BasicVertex>(defaultBasicShader);

		defaultColoredShader   = createShader("ColoredMesh.vert.spv", "ColoredMesh.frag.spv");
		defaultColoredMaterial = createMaterial<ColoredVertex>(defaultColoredShader);

		defaultTexturedShader   = createShader("TexturedMesh.vert.spv", "TexturedMesh.frag.spv");
		defaultTexturedMaterial = createMaterial<TexturedVertex>(defaultTexturedShader);
	}

	void VkRenderer::destroySwapchain()
	{
		if (!isInitalized) { return; }

		m_device.destroyRenderPass(m_renderPass);
		for (const auto& framebuffer : m_framebuffers) { m_device.destroyFramebuffer(framebuffer); }
		m_device.destroySwapchainKHR(m_swapchain);
		for (const auto& imageView : m_swapchainImageViews) { m_device.destroyImageView(imageView); }
		vmaDestroyImage(m_allocator, m_depthImage.image, m_depthImage.allocation);
		m_device.destroyImageView(m_depthImageView);
	}
}  // namespace MRG
