//
// Created by mathi on 2021-04-11.
//

#include "VkRenderer.h"

#include "Rendering/VkInitialize.h"

#include <VkBootstrap.h>

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
	void VkRenderer::init(const RendererSpecification& newSpec, GLFWwindow* newWindow)
	{
		spec   = newSpec;
		window = newWindow;

		initVulkan();
		initSwapchain();
		initCommands();

		isInitalized = true;
	}

	void VkRenderer::cleanup()
	{
		if (!isInitalized) { return; }

		m_device.destroyCommandPool(m_cmdPool);

		destroySwapchain();

		m_device.destroyRenderPass(m_renderPass);
		for (auto& framebuffer : m_framebuffers) { m_device.destroyFramebuffer(framebuffer); }
		m_device.destroy();

		m_instance.destroySurfaceKHR(m_surface);
		vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
		m_instance.destroy();

		glfwDestroyWindow(window);
	}

	void VkRenderer::onResize()
	{
		MRG_ENGINE_TRACE("Recreating swapchain")

		destroySwapchain();
		initSwapchain();
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
	}

	void VkRenderer::initSwapchain()
	{
		vkb::SwapchainBuilder swapchainBuilder{m_GPU, m_device, m_surface};
		auto vkbSwapchain = swapchainBuilder.use_default_format_selection()
		                      .set_desired_present_mode(spec.presentMode)
		                      .set_desired_extent(spec.windowWidth, spec.windowHeight)
		                      .build()
		                      .value();

		m_swapchain       = vkbSwapchain.swapchain;
		m_swapchainFormat = vk::Format{vkbSwapchain.image_format};

		auto rawImages        = vkbSwapchain.get_images().value();
		auto rawImageViews    = vkbSwapchain.get_image_views().value();
		m_swapchainImages     = std::vector<vk::Image>(rawImages.begin(), rawImages.end());
		m_swapchainImageViews = std::vector<vk::ImageView>(rawImageViews.begin(), rawImageViews.end());
	}

	void VkRenderer::initCommands()
	{
		// create command pool
		const auto cmdPoolInfo = VkInit::cmdPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_graphicsQueueIndex);
		m_cmdPool              = m_device.createCommandPool(cmdPoolInfo);

		// allocate main command buffer from created command pool
		const auto mainCmdBufferInfo = VkInit::cmdBufferAllocateInfo(m_cmdPool, vk::CommandBufferLevel::ePrimary, 1);
		m_mainCmdBuffer              = m_device.allocateCommandBuffers(mainCmdBufferInfo)[0];
	}

	void VkRenderer::initDefaultRenderPass()
	{
		vk::AttachmentDescription colorAttachment{.format         = m_swapchainFormat,
		                                          .samples        = vk::SampleCountFlagBits::e1,
		                                          .loadOp         = vk::AttachmentLoadOp::eClear,
		                                          .storeOp        = vk::AttachmentStoreOp::eStore,
		                                          .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
		                                          .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		                                          .initialLayout  = vk::ImageLayout::eUndefined,
		                                          .finalLayout    = vk::ImageLayout::ePresentSrcKHR};

		vk::AttachmentReference colorAttachmentRef{.attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal};

		vk::SubpassDescription subpass{
		  .pipelineBindPoint = vk::PipelineBindPoint::eGraphics, .colorAttachmentCount = 1, .pColorAttachments = &colorAttachmentRef};

		vk::RenderPassCreateInfo renderPassInfo{
		  .attachmentCount = 1, .pAttachments = &colorAttachment, .subpassCount = 1, .pSubpasses = &subpass};

		m_renderPass = m_device.createRenderPass(renderPassInfo);
	}

	void VkRenderer::initFramebuffers()
	{
		vk::FramebufferCreateInfo framebufferInfo{.renderPass      = m_renderPass,
		                                          .attachmentCount = 1,
		                                          .width           = static_cast<uint32_t>(spec.windowWidth),
		                                          .height          = static_cast<uint32_t>(spec.windowHeight),
		                                          .layers          = 1};

		const std::size_t swapchainImageCount = m_swapchainImages.size();
		m_framebuffers                        = std::vector<vk::Framebuffer>(swapchainImageCount);

		for (auto i = 0; i < swapchainImageCount; ++i) {
			framebufferInfo.pAttachments = &m_swapchainImageViews[i];
			m_framebuffers[i]            = m_device.createFramebuffer(framebufferInfo);
		}
	}

	void VkRenderer::destroySwapchain()
	{
		if (!isInitalized) { return; }

		m_device.destroySwapchainKHR(m_swapchain);
		for (const auto& imageView : m_swapchainImageViews) { m_device.destroyImageView(imageView); }
	}
}  // namespace MRG
