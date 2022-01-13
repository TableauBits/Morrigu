//
// Created by Mathis Lamidey on 2021-04-11.
//

#include "Renderer.h"

#include "Vendor/ImGui/bindings/imgui_impl_glfw.h"
#include "Vendor/ImGui/bindings/imgui_impl_vulkan.h"
#include <VkBootstrap.h>
#include <imgui.h>

DISABLE_WARNING_PUSH
DISABLE_WARNING_ALIGNMENT_MODIFIED
#include <filesystem>
DISABLE_WARNING_POP
#include <fstream>

namespace
{
	[[maybe_unused]] VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

	std::string getVendorString(uint32_t vendorID)
	{
		switch (vendorID) {
		case 0x1002:
			return "AMD";
		case 0x1010:
			return "ImgTec";
		case 0x10DE:
			return "NVIDIA";
		case 0x13B5:
			return "ARM";
		case 0x5143:
			return "Qualcomm";
		case 0x8086:  // cheeky
			return "INTEL";

		default:
			return "unknown";
		}
	}

	std::string getDeviceTypeString(vk::PhysicalDeviceType type)
	{
		switch (type) {
		case vk::PhysicalDeviceType::eOther:
			return "Other";
		case vk::PhysicalDeviceType::eIntegratedGpu:
			return "Integrated GPU";
		case vk::PhysicalDeviceType::eDiscreteGpu:
			return "Discrete GPU";
		case vk::PhysicalDeviceType::eVirtualGpu:
			return "Virtual GPU";
		case vk::PhysicalDeviceType::eCpu:
			return "CPU";

		default:
			return "unknown";
		}
	}
}  // namespace

namespace MRG
{
	Renderer::Renderer(const RendererSpecification& newSpec, GLFWwindow* newWindow)
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
		initImGui();

		isInitalized = true;
	}

	Renderer::~Renderer()
	{
		m_device.waitIdle();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		m_device.destroyDescriptorPool(m_imGuiPool);

		const auto newPipelineCacheData = m_device.getPipelineCacheData(m_pipelineCache);
		std::ofstream pipelineCacheFile{Files::Rendering::vkPipelineCacheFile, std::ios::binary | std::ios::trunc};
		pipelineCacheFile.write(reinterpret_cast<const char*>(newPipelineCacheData.data()),
		                        static_cast<std::streamsize>(newPipelineCacheData.size()));
		m_device.destroyPipelineCache(m_pipelineCache);

		m_device.destroyDescriptorSetLayout(m_level1DSL);
		m_device.destroyDescriptorSetLayout(m_level0DSL);
		m_device.destroyDescriptorPool(m_descriptorPool);

		m_device.destroyFence(m_uploadContext.uploadFence);
		for (auto& frameData : m_framesData) {
			m_device.destroySemaphore(frameData.presentSemaphore);
			m_device.destroySemaphore(frameData.renderSemaphore);
			m_device.destroyFence(frameData.renderFence);

			m_device.destroyCommandPool(frameData.commandPool);
		}

		m_device.destroyCommandPool(m_uploadContext.commandPool);
	}

	Ref<Shader> Renderer::createShader(const char* vertexShaderName, const char* fragmentShaderName)
	{
		return MRG::createRef<Shader>(m_device, vertexShaderName, fragmentShaderName);
	}

	Ref<Texture> Renderer::createTexture(void* data, uint32_t width, uint32_t height)
	{
		return createRef<Texture>(m_device, m_graphicsQueue, m_uploadContext, m_allocator, data, width, height);
	}

	Ref<Texture> Renderer::createTexture(const char* fileName)
	{
		return createRef<Texture>(m_device, m_graphicsQueue, m_uploadContext, m_allocator, fileName);
	}

	Ref<Framebuffer> Renderer::createFrameBuffer(const FramebufferSpecification& fbSpec)
	{
		Framebuffer::VulkanObjects objs{
		  .device             = m_device,
		  .graphicsQueue      = m_graphicsQueue,
		  .uploadContext      = m_uploadContext,
		  .allocator          = m_allocator,
		  .swapchainFormat    = m_swapchainFormat,
		  .depthImageFormat   = m_depthImage.spec.format,
		  .renderPass         = m_fbRenderPass,
		  .graphicsQueueIndex = m_graphicsQueueIndex,
		  .level0DSL          = m_level0DSL,
		};
		return createRef<Framebuffer>(fbSpec, objs);
	}

	AllocatedBuffer Renderer::createBuffer(std::size_t bufferSize, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage allocationUsage)
	{
		return AllocatedBuffer{m_allocator, bufferSize, bufferUsage, allocationUsage};
	}

	void Renderer::bindBufferToSet1(AllocatedBuffer& buffer, std::size_t bufferSize)
	{
		vk::DescriptorBufferInfo bufferInfo{
		  .buffer = buffer.vkHandle,
		  .offset = 0,
		  .range  = bufferSize,
		};
		vk::WriteDescriptorSet set1Write{
		  .dstSet          = m_level1Descriptor,
		  .dstBinding      = 0,
		  .descriptorCount = 1,
		  .descriptorType  = vk::DescriptorType::eUniformBuffer,
		  .pBufferInfo     = &bufferInfo,
		};

		m_device.updateDescriptorSets(set1Write, {});
	}

	bool Renderer::beginFrame()
	{
		const auto& frameData = getCurrentFrameData();
		MRG_VK_CHECK_HPP(m_device.waitForFences(frameData.renderFence, VK_TRUE, UINT64_MAX), "failed to wait for render fence!")
		m_device.resetFences(frameData.renderFence);

		try {
			m_imageIndex = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, frameData.presentSemaphore).value;
		} catch (const vk::OutOfDateKHRError&) {
			onResize();
			vk::SubmitInfo fenceReset{};
			m_graphicsQueue.submit(fenceReset, frameData.renderFence);
			return false;
		}

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

		return true;
	}

	void Renderer::endFrame()
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

		try {
			MRG_VK_CHECK_HPP(m_graphicsQueue.presentKHR(presentInfo), "failed to present image to screen!")
		} catch (const vk::OutOfDateKHRError&) {}
		++m_frameNumber;
	}

	void Renderer::beginImGui()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();
	}

	void Renderer::endImGui()
	{
		const auto& frameData = getCurrentFrameData();

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frameData.commandBuffer);

		if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void Renderer::onResize()
	{
		m_device.waitIdle();

		destroySwapchain();
		initSwapchain();
		initFramebuffers();
	}

	void Renderer::initVulkan()
	{
		std::array<uint32_t, 3> requestedAPIVersion{1, 0, 0};

		// basic instance creation
		vkb::InstanceBuilder instanceBuilder{};
		const auto vkbInstance =
		  instanceBuilder.set_app_name(spec.applicationName.c_str())
		    .require_api_version(requestedAPIVersion[0], requestedAPIVersion[1], requestedAPIVersion[2])
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
		glfwCreateWindowSurface(m_instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_surface));

		// GPU selection
		vkb::PhysicalDeviceSelector selector{vkbInstance};
		const auto vkbPhysicalDevice =
		  selector.set_minimum_version(requestedAPIVersion[0], requestedAPIVersion[1]).set_surface(m_surface).select().value();

		m_GPU = vkbPhysicalDevice.physical_device;

		const auto properties = m_GPU.getProperties();
		MRG_ENGINE_INFO("Selected device: {}", properties.deviceName)
		MRG_ENGINE_TRACE("\tVendor: {}", getVendorString(properties.vendorID))
		MRG_ENGINE_TRACE("\tType: {}", getDeviceTypeString(properties.deviceType))
		MRG_ENGINE_TRACE("\tSupported API version: {}.{}.{} (requested {}.{}.{})",
		                 VK_VERSION_MAJOR(properties.apiVersion),
		                 VK_VERSION_MINOR(properties.apiVersion),
		                 VK_VERSION_PATCH(properties.apiVersion),
		                 requestedAPIVersion[0],
		                 requestedAPIVersion[1],
		                 requestedAPIVersion[2])

		// VkDevice creation
		vkb::DeviceBuilder deviceBuilder{vkbPhysicalDevice};
		const auto vkbDevice = deviceBuilder.build().value();

		m_device             = vkbDevice.device;
		m_graphicsQueue      = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_graphicsQueueIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		// VMA allocator creation
		VmaAllocatorCreateInfo allocatorInfo{
		  .flags                       = 0,
		  .physicalDevice              = m_GPU,
		  .device                      = m_device,
		  .preferredLargeHeapBlockSize = 0,
		  .pAllocationCallbacks        = nullptr,
		  .pDeviceMemoryCallbacks      = nullptr,
		  .frameInUseCount             = 0,
		  .pHeapSizeLimit              = nullptr,
		  .pVulkanFunctions            = nullptr,
		  .pRecordSettings             = nullptr,
		  .instance                    = m_instance,
		  .vulkanApiVersion            = 0,
		};
		MRG_VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator), "failed to create VMA allocator!")
	}

	void Renderer::initSwapchain()
	{
		vkb::SwapchainBuilder swapchainBuilder{m_GPU, m_device, m_surface};
		auto vkbSwapchain = swapchainBuilder
		                      .set_desired_format({
		                        .format     = static_cast<VkFormat>(vk::Format::eB8G8R8A8Srgb),
		                        .colorSpace = static_cast<VkColorSpaceKHR>(vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear),
		                      })
		                      .set_desired_present_mode(static_cast<VkPresentModeKHR>(spec.preferredPresentMode))
		                      .set_desired_extent(static_cast<uint32_t>(spec.windowWidth), static_cast<uint32_t>(spec.windowHeight))
		                      .build()
		                      .value();

		spec.windowWidth  = static_cast<int>(vkbSwapchain.extent.width);
		spec.windowHeight = static_cast<int>(vkbSwapchain.extent.height);

		m_swapchain       = vkbSwapchain.swapchain;
		m_imageCount      = vkbSwapchain.image_count;
		m_swapchainFormat = vk::Format{vkbSwapchain.image_format};

		const auto rawImages     = vkbSwapchain.get_images().value();
		const auto rawImageViews = vkbSwapchain.get_image_views().value();
		m_swapchainImages        = std::vector<vk::Image>(rawImages.begin(), rawImages.end());
		m_swapchainImageViews    = std::vector<vk::ImageView>(rawImageViews.begin(), rawImageViews.end());

		m_depthImage.spec.allocator = m_allocator;
		m_depthImage.spec.device    = m_device;
		m_depthImage.spec.format    = vk::Format::eD32Sfloat;
		vk::Extent3D depthImageExtent{
		  .width  = static_cast<uint32_t>(spec.windowWidth),
		  .height = static_cast<uint32_t>(spec.windowHeight),
		  .depth  = 1,
		};
		VkImageCreateInfo depthImageCreateInfo{
		  .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		  .pNext                 = nullptr,
		  .flags                 = 0,
		  .imageType             = VK_IMAGE_TYPE_2D,
		  .format                = static_cast<VkFormat>(m_depthImage.spec.format),
		  .extent                = depthImageExtent,
		  .mipLevels             = 1,
		  .arrayLayers           = 1,
		  .samples               = VK_SAMPLE_COUNT_1_BIT,
		  .tiling                = VK_IMAGE_TILING_OPTIMAL,
		  .usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		  .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
		  .queueFamilyIndexCount = 0,
		  .pQueueFamilyIndices   = nullptr,
		  .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
		};
		VmaAllocationCreateInfo depthImageAllocationCreateInfo{
		  .flags          = 0,
		  .usage          = VMA_MEMORY_USAGE_GPU_ONLY,
		  .requiredFlags  = VkMemoryPropertyFlags{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT},
		  .preferredFlags = 0,
		  .memoryTypeBits = 0,
		  .pool           = VK_NULL_HANDLE,
		  .pUserData      = nullptr,
		};

		VkImage rawImage;
		vmaCreateImage(m_allocator, &depthImageCreateInfo, &depthImageAllocationCreateInfo, &rawImage, &m_depthImage.allocation, nullptr);
		m_depthImage.vkHandle = rawImage;

		vk::ImageViewCreateInfo depthImageViewCreateInfo{
		  .image    = m_depthImage.vkHandle,
		  .viewType = vk::ImageViewType::e2D,
		  .format   = m_depthImage.spec.format,
		  .subresourceRange{
		    .aspectMask     = vk::ImageAspectFlagBits::eDepth,
		    .baseMipLevel   = 0,
		    .levelCount     = 1,
		    .baseArrayLayer = 0,
		    .layerCount     = 1,
		  },
		};
		m_depthImage.view = m_device.createImageView(depthImageViewCreateInfo);
	}

	void Renderer::initCommands()
	{
		// create command pool
		vk::CommandPoolCreateInfo cmdPoolInfo{
		  .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		  .queueFamilyIndex = m_graphicsQueueIndex,
		};

		for (auto& frame : m_framesData) {
			frame.commandPool = m_device.createCommandPool(cmdPoolInfo);

			// allocate main command buffer from created command pool
			vk::CommandBufferAllocateInfo mainCmdBufferInfo{
			  .commandPool        = frame.commandPool,
			  .level              = vk::CommandBufferLevel::ePrimary,
			  .commandBufferCount = 1,
			};
			frame.commandBuffer = m_device.allocateCommandBuffers(mainCmdBufferInfo)[0];
		}

		// create upload context command pool
		cmdPoolInfo.flags           = {};
		m_uploadContext.commandPool = m_device.createCommandPool(cmdPoolInfo);
	}

	void Renderer::initDefaultRenderPass()
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
		  .format         = m_depthImage.spec.format,
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

		attachments[0].finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		m_fbRenderPass             = m_device.createRenderPass(renderPassInfo);
	}

	void Renderer::initFramebuffers()
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

		for (std::size_t i = 0; i < swapchainImageCount; ++i) {
			std::array<vk::ImageView, 2> attachments{m_swapchainImageViews[i], m_depthImage.view};
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments    = attachments.data();

			m_framebuffers[i] = m_device.createFramebuffer(framebufferInfo);
		}
	}

	void Renderer::initSyncSructs()
	{
		vk::FenceCreateInfo fenceInfo{
		  .flags = vk::FenceCreateFlagBits::eSignaled,
		};

		for (auto& frame : m_framesData) {
			frame.renderFence = m_device.createFence(fenceInfo);

			frame.presentSemaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
			frame.renderSemaphore  = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
		}

		fenceInfo.flags             = {};
		m_uploadContext.uploadFence = m_device.createFence(fenceInfo);
	}

	void Renderer::initDescriptors()
	{
		std::array<vk::DescriptorPoolSize, 1> sizes{{vk::DescriptorType::eUniformBuffer, 1}};
		vk::DescriptorPoolCreateInfo poolInfo{
		  .maxSets       = FRAMES_IN_FLIGHT + 1,
		  .poolSizeCount = static_cast<uint32_t>(sizes.size()),
		  .pPoolSizes    = sizes.data(),
		};
		m_descriptorPool = m_device.createDescriptorPool(poolInfo);

		std::array<vk::DescriptorSetLayoutBinding, 1> level0Bindings{
		  vk::DescriptorSetLayoutBinding{
		    .binding         = 0,
		    .descriptorType  = vk::DescriptorType::eUniformBuffer,
		    .descriptorCount = 1,
		    .stageFlags      = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
		  },
		};
		vk::DescriptorSetLayoutCreateInfo setInfo{
		  .bindingCount = static_cast<uint32_t>(level0Bindings.size()),
		  .pBindings    = level0Bindings.data(),
		};
		m_level0DSL = m_device.createDescriptorSetLayout(setInfo);

		std::array<vk::DescriptorSetLayout, FRAMES_IN_FLIGHT> layouts{};
		layouts.fill(m_level0DSL);
		vk::DescriptorSetAllocateInfo allocInfo{
		  .descriptorPool     = m_descriptorPool,
		  .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
		  .pSetLayouts        = layouts.data(),
		};
		const auto level0Descriptors = m_device.allocateDescriptorSets(allocInfo);

		vk::DescriptorBufferInfo timeBufferInfo{
		  .offset = 0,
		  .range  = sizeof(TimeData),
		};
		vk::WriteDescriptorSet timeSetWrite{
		  .dstBinding      = 0,
		  .descriptorCount = 1,
		  .descriptorType  = vk::DescriptorType::eUniformBuffer,
		  .pBufferInfo     = &timeBufferInfo,
		};

		for (std::size_t i = 0; i < FRAMES_IN_FLIGHT; ++i) {
			m_framesData[i].timeDataBuffer =
			  AllocatedBuffer{m_allocator, sizeof(TimeData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU};
			m_framesData[i].level0Descriptor = level0Descriptors[i];

			timeBufferInfo.buffer = m_framesData[i].timeDataBuffer.vkHandle;
			timeSetWrite.dstSet   = m_framesData[i].level0Descriptor;
			m_device.updateDescriptorSets(timeSetWrite, {});
		}

		// level 1 DSL
		setInfo.bindingCount = 0;
		m_level1DSL          = m_device.createDescriptorSetLayout(spec.level1SetInfo);

		vk::DescriptorSetAllocateInfo allocLeve1Info{
		  .descriptorPool     = m_descriptorPool,
		  .descriptorSetCount = 1,
		  .pSetLayouts        = &m_level1DSL,
		};
		m_level1Descriptor = m_device.allocateDescriptorSets(allocLeve1Info)[0];
	}

	void Renderer::initAssets() { defaultTexture = createTexture(Files::Rendering::defaultTexture.c_str()); }

	void Renderer::initMaterials()
	{
		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo{};
		if (std::filesystem::exists(Files::Rendering::vkPipelineCacheFile)) {
			MRG_ENGINE_TRACE("Pipeline cache found.")
			std::ifstream pipelineCacheFile{Files::Rendering::vkPipelineCacheFile, std::ios::binary | std::ios::ate};
			const auto fileSize = static_cast<std::size_t>(pipelineCacheFile.tellg());
			std::vector<uint8_t> pipelineData(fileSize / sizeof(uint8_t));
			pipelineCacheFile.seekg(std::ios::beg);
			pipelineCacheFile.read(reinterpret_cast<char*>(pipelineData.data()), static_cast<std::streamsize>(fileSize));
			pipelineCacheFile.close();
		}
		m_pipelineCache = m_device.createPipelineCache(pipelineCacheCreateInfo);

		defaultBasicShader   = createShader("BasicMesh.vert.spv", "BasicMesh.frag.spv");
		defaultBasicMaterial = createMaterial<BasicVertex>(defaultBasicShader, {});

		defaultColoredShader   = createShader("ColoredMesh.vert.spv", "ColoredMesh.frag.spv");
		defaultColoredMaterial = createMaterial<ColoredVertex>(defaultColoredShader, {});

		defaultTexturedShader   = createShader("TexturedMesh.vert.spv", "TexturedMesh.frag.spv");
		defaultTexturedMaterial = createMaterial<TexturedVertex>(defaultTexturedShader, {});

		pbrShader   = createShader("PBR.vert.spv", "PBR.frag.spv");
		pbrMaterial = createMaterial<TexturedVertex>(pbrShader, {});
	}

	void Renderer::initImGui()
	{
		std::array<vk::DescriptorPoolSize, 11> poolSizes{
		  vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 1000},
		  vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 1000},
		};

		vk::DescriptorPoolCreateInfo imGuiPoolInfo{
		  .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		  .maxSets       = 1000,
		  .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
		  .pPoolSizes    = poolSizes.data(),
		};

		m_imGuiPool = m_device.createDescriptorPool(imGuiPoolInfo);

		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
		ImGui_ImplVulkan_InitInfo initInfo{
		  .Instance        = m_instance,
		  .PhysicalDevice  = m_GPU,
		  .Device          = m_device,
		  .QueueFamily     = 0,
		  .Queue           = m_graphicsQueue,
		  .PipelineCache   = m_pipelineCache,
		  .DescriptorPool  = m_imGuiPool,
		  .Subpass         = 0,
		  .MinImageCount   = m_imageCount,
		  .ImageCount      = m_imageCount,
		  .MSAASamples     = VK_SAMPLE_COUNT_1_BIT,
		  .Allocator       = nullptr,
		  .CheckVkResultFn = nullptr,
		};

		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_Init(&initInfo, m_renderPass);

		Utils::Commands::immediateSubmit(
		  m_device, m_graphicsQueue, m_uploadContext, [&](vk::CommandBuffer cmdBuffer) { ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer); });
		ImGui_ImplVulkan_DestroyFontUploadObjects();

		ImGui::StyleColorsDark();

		// Set up style
		// (generated from https://github.com/ocornut/imgui/issues/2265#issuecomment-465432091)
		ImGui::GetStyle().WindowPadding        = {0, 0};
		ImGui::GetStyle().WindowBorderSize     = 0;
		ImGui::GetStyle().ChildBorderSize      = 0;
		ImGui::GetStyle().FrameBorderSize      = 0;
		ImGui::GetStyle().PopupBorderSize      = 0;
		ImGui::GetStyle().TabBorderSize        = 0;
		ImVec4* colors                         = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text]                  = ImVec4(0.96f, 0.77f, 0.80f, 0.78f);
		colors[ImGuiCol_TextDisabled]          = ImVec4(0.96f, 0.77f, 0.80f, 0.28f);
		colors[ImGuiCol_WindowBg]              = ImVec4(-0.37f, -0.26f, -0.32f, 1.00f);
		colors[ImGuiCol_ChildBg]               = ImVec4(0.03f, 0.02f, 0.02f, 0.58f);
		colors[ImGuiCol_PopupBg]               = ImVec4(0.03f, 0.02f, 0.02f, 0.90f);
		colors[ImGuiCol_Border]                = ImVec4(-0.02f, -0.02f, -0.02f, 0.60f);
		colors[ImGuiCol_BorderShadow]          = ImVec4(0.03f, 0.02f, 0.02f, 0.00f);
		colors[ImGuiCol_FrameBg]               = ImVec4(0.03f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.17f, 0.04f, 0.07f, 0.78f);
		colors[ImGuiCol_FrameBgActive]         = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.02f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive]         = ImVec4(0.14f, 0.03f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.03f, 0.02f, 0.02f, 0.75f);
		colors[ImGuiCol_MenuBarBg]             = ImVec4(0.03f, 0.02f, 0.02f, 0.47f);
		colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.03f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.02f, 0.04f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.17f, 0.04f, 0.07f, 0.78f);
		colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark]             = ImVec4(0.14f, 0.03f, 0.06f, 1.00f);
		colors[ImGuiCol_SliderGrab]            = ImVec4(0.13f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.14f, 0.03f, 0.06f, 1.00f);
		colors[ImGuiCol_Button]                = ImVec4(0.23f, 0.16f, 0.20f, 1.00f);
		colors[ImGuiCol_ButtonHovered]         = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_ButtonActive]          = ImVec4(0.14f, 0.03f, 0.06f, 1.00f);
		colors[ImGuiCol_Header]                = ImVec4(0.17f, 0.04f, 0.07f, 0.76f);
		colors[ImGuiCol_HeaderHovered]         = ImVec4(0.17f, 0.04f, 0.07f, 0.86f);
		colors[ImGuiCol_HeaderActive]          = ImVec4(0.14f, 0.03f, 0.06f, 1.00f);
		colors[ImGuiCol_Separator]             = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive]       = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
		colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.17f, 0.04f, 0.07f, 0.78f);
		colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_Tab]                   = ImVec4(0.03f, 0.02f, 0.02f, 0.40f);
		colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.03f, 0.06f, 1.00f);
		colors[ImGuiCol_TabActive]             = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_TabUnfocused]          = ImVec4(0.03f, 0.02f, 0.02f, 0.40f);
		colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.03f, 0.02f, 0.02f, 0.70f);
		colors[ImGuiCol_DockingPreview]        = ImVec4(0.14f, 0.03f, 0.06f, 0.30f);
		colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines]             = ImVec4(0.96f, 0.77f, 0.80f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_PlotHistogram]         = ImVec4(0.96f, 0.77f, 0.80f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.17f, 0.04f, 0.07f, 1.00f);
		colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.17f, 0.04f, 0.07f, 0.43f);
		colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.03f, 0.02f, 0.02f, 0.73f);
	}

	void Renderer::destroySwapchain()
	{
		if (!isInitalized) { return; }

		for (const auto& framebuffer : m_framebuffers) { m_device.destroyFramebuffer(framebuffer); }
		m_device.destroySwapchainKHR(m_swapchain);
		for (const auto& imageView : m_swapchainImageViews) { m_device.destroyImageView(imageView); }
		vmaDestroyImage(m_allocator, m_depthImage.vkHandle, m_depthImage.allocation);
		m_device.destroyImageView(m_depthImage.view);
	}
}  // namespace MRG
