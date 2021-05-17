//
// Created by Mathis Lamidey on 2021-04-11.
//

#include "VkRenderer.h"

#include "Core/FileNames.h"
#include "Rendering/PipelineBuilder.h"
#include "Rendering/VkInitialize.h"

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
		initPipelines();

		isInitalized = true;
	}

	void VkRenderer::beginFrame()
	{
		MRG_VK_CHECK_HPP(m_device.waitForFences(m_renderFence, VK_TRUE, UINT64_MAX), "failed to wait for render fence!")
		m_device.resetFences(m_renderFence);

		m_imageIndex = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, m_presentSemaphore).value;

		m_mainCmdBuffer.reset();
		vk::CommandBufferBeginInfo beginInfo{
		  .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};
		m_mainCmdBuffer.begin(beginInfo);

		vk::ClearValue clearValue{{std::array<float, 4>{0.f, 0.f, std::abs(std::sin(static_cast<float>(frameNumber) / 120.f)), 1.f}}};

		vk::RenderPassBeginInfo renderPassInfo{
		  .renderPass  = m_renderPass,
		  .framebuffer = m_framebuffers[m_imageIndex],
		  .renderArea =
		    vk::Rect2D{
		      .offset = {0, 0},
		      .extent = {static_cast<uint32_t>(spec.windowWidth), static_cast<uint32_t>(spec.windowHeight)},
		    },
		  .clearValueCount = 1,
		  .pClearValues    = &clearValue};
		m_mainCmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	}

	void VkRenderer::endFrame()
	{
		m_mainCmdBuffer.endRenderPass();
		m_mainCmdBuffer.end();

		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo submitInfo{
		  .waitSemaphoreCount   = 1,
		  .pWaitSemaphores      = &m_presentSemaphore,
		  .pWaitDstStageMask    = &waitStage,
		  .commandBufferCount   = 1,
		  .pCommandBuffers      = &m_mainCmdBuffer,
		  .signalSemaphoreCount = 1,
		  .pSignalSemaphores    = &m_renderSemaphore,
		};
		m_graphicsQueue.submit(submitInfo, m_renderFence);

		vk::PresentInfoKHR presentInfo{
		  .waitSemaphoreCount = 1,
		  .pWaitSemaphores    = &m_renderSemaphore,
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
		MRG_VK_CHECK_HPP(m_device.waitForFences(m_renderFence, VK_TRUE, UINT64_MAX), "Failed to wait for render fence!")

		destroySwapchain();
		initSwapchain();
		initCommands();
		initDefaultRenderPass();
		initFramebuffers();
	}

	vk::ShaderModule VkRenderer::loadShaderModule(const char* filePath)
	{
		MRG_ENGINE_ASSERT(std::filesystem::exists(filePath), "Shader file \"{}\" does not exists!", filePath)
		std::ifstream file{filePath, std::ios::binary | std::ios::ate};
		const auto fileSize = static_cast<std::size_t>(file.tellg());
		std::vector<std::uint32_t> buffer(fileSize / sizeof(std::uint32_t));
		file.seekg(std::ios::beg);
		file.read((char*)buffer.data(), static_cast<std::streamsize>(fileSize));
		file.close();

		vk::ShaderModuleCreateInfo moduleInfo{
		  .codeSize = fileSize,
		  .pCode    = buffer.data(),
		};
		return m_device.createShaderModule(moduleInfo);
	}

	void VkRenderer::uploadMesh(Mesh& mesh)
	{
		VkBufferCreateInfo bufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		                              .size  = mesh.vertices.size() * sizeof(Vertex),
		                              .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT};
		VkBuffer newRawBuffer;

		VmaAllocationCreateInfo allocationInfo{.usage = VMA_MEMORY_USAGE_CPU_TO_GPU};
		MRG_VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &allocationInfo, &newRawBuffer, &mesh.vertexBuffer.allocation, nullptr), "")
		mesh.vertexBuffer.buffer = newRawBuffer;
		m_deletionQueue.push([=]() { vmaDestroyBuffer(m_allocator, mesh.vertexBuffer.buffer, mesh.vertexBuffer.allocation); });

		void* data;
		vmaMapMemory(m_allocator, mesh.vertexBuffer.allocation, &data);
		memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
		vmaUnmapMemory(m_allocator, mesh.vertexBuffer.allocation);
	}

	void VkRenderer::drawMesh(const Mesh& mesh, const Camera& camera)
	{
		// @TODO(Ithyx): check if we are currently in a RP
		m_mainCmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_coloredMeshPipeline);
		m_mainCmdBuffer.setViewport(0,
		                            vk::Viewport{
		                              .x        = 0.f,
		                              .y        = 0.f,
		                              .width    = static_cast<float>(spec.windowWidth),
		                              .height   = static_cast<float>(spec.windowHeight),
		                              .minDepth = 0.f,
		                              .maxDepth = 1.f,
		                            });
		m_mainCmdBuffer.setScissor(0,
		                           vk::Rect2D{
		                             .offset{0, 0},
		                             .extent = {static_cast<uint32_t>(spec.windowWidth), static_cast<uint32_t>(spec.windowHeight)},
		                           });

		const auto pushConstants = Mesh::PushConstants{
		  .data      = glm::vec4{},
		  .transform = camera.getViewProjection() * mesh.modelMatrix,
		};
		m_mainCmdBuffer.pushConstants(
		  m_coloredMeshPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mesh::PushConstants), (void*)(&pushConstants));

		m_mainCmdBuffer.bindVertexBuffers(0, mesh.vertexBuffer.buffer, {0});
		m_mainCmdBuffer.draw(static_cast<uint32_t>(mesh.vertices.size()), 1, 0, 0);
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
		                      .set_desired_present_mode(static_cast<VkPresentModeKHR>(spec.presentMode))
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

		vk::SubpassDescription subpass{
		  .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
		  .colorAttachmentCount = 1,
		  .pColorAttachments    = &colorAttachmentRef,
		};

		vk::RenderPassCreateInfo renderPassInfo{
		  .attachmentCount = 1,
		  .pAttachments    = &colorAttachment,
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
			framebufferInfo.pAttachments = &m_swapchainImageViews[i];
			m_framebuffers[i]            = m_device.createFramebuffer(framebufferInfo);
		}
	}

	void VkRenderer::initSyncSructs()
	{
		vk::FenceCreateInfo fenceInfo{
		  .flags = vk::FenceCreateFlagBits::eSignaled,
		};
		m_renderFence = m_device.createFence(fenceInfo);

		m_deletionQueue.push([=]() { m_device.destroyFence(m_renderFence); });

		m_presentSemaphore = m_device.createSemaphore(vk::SemaphoreCreateInfo{});
		m_renderSemaphore  = m_device.createSemaphore(vk::SemaphoreCreateInfo{});

		m_deletionQueue.push([=]() {
			m_device.destroySemaphore(m_presentSemaphore);
			m_device.destroySemaphore(m_renderSemaphore);
		});
	}

	void VkRenderer::initPipelines()
	{
		auto coloredMeshVertShader = loadShaderModule("shaders/ColoredMesh.vert.spv");
		auto coloredMeshFragShader = loadShaderModule("shaders/ColoredMesh.frag.spv");
		MRG_ENGINE_TRACE("Loaded vertex and fragment shaders")

		vk::PushConstantRange pushConstantRange{
		  .stageFlags = vk::ShaderStageFlagBits::eVertex,
		  .offset     = 0,
		  .size       = sizeof(Mesh::PushConstants),
		};
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
		  .pushConstantRangeCount = 1,
		  .pPushConstantRanges    = &pushConstantRange,
		};
		m_coloredMeshPipelineLayout = m_device.createPipelineLayout(pipelineLayoutInfo);

		m_deletionQueue.push([=]() { m_device.destroyPipelineLayout(m_coloredMeshPipelineLayout); });

		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo{};
		if (std::filesystem::exists(Files::Rendering::VkPipelineCacheFN)) {
			MRG_ENGINE_TRACE("Pipeline cache found.")
			std::ifstream pipelineCacheFile{Files::Rendering::VkPipelineCacheFN, std::ios::binary | std::ios::ate};
			const auto fileSize = static_cast<std::size_t>(pipelineCacheFile.tellg());
			std::vector<uint8_t> pipelineData(fileSize / sizeof(uint8_t));
			pipelineCacheFile.seekg(std::ios::beg);
			pipelineCacheFile.read((char*)pipelineData.data(), static_cast<std::streamsize>(fileSize));
			pipelineCacheFile.close();
		}
		m_pipelineCache = m_device.createPipelineCache(pipelineCacheCreateInfo);

		m_deletionQueue.push([=]() {
			const auto newPipelineCacheData = m_device.getPipelineCacheData(m_pipelineCache);
			std::ofstream pipelineCacheFile{Files::Rendering::VkPipelineCacheFN, std::ios::binary | std::ios::trunc};
			pipelineCacheFile.write((char*)newPipelineCacheData.data(), static_cast<std::streamsize>(newPipelineCacheData.size()));
			m_device.destroyPipelineCache(m_pipelineCache);
		});

		const auto vertexInfo = Vertex::getVertexDescription();
		vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo{
		  .vertexBindingDescriptionCount   = static_cast<uint32_t>(vertexInfo.bindings.size()),
		  .pVertexBindingDescriptions      = vertexInfo.bindings.data(),
		  .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInfo.attributes.size()),
		  .pVertexAttributeDescriptions    = vertexInfo.attributes.data(),
		};

		PipelineBuilder builder{
		  .shaderStages{VkInit::pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits::eVertex, coloredMeshVertShader),
		                VkInit::pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits::eFragment, coloredMeshFragShader)},
		  .vertexInputInfo{vertexInputCreateInfo},
		  .inputAssemblyInfo{VkInit::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList)},
		  .rasterizerInfo{VkInit::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill)},
		  .multisamplingInfo{VkInit::pipelineMultisampleStateCreateInfo()},
		  .colorBlendAttachment{VkInit::pipelineColorBlendAttachmentState()},
		  .pipelineLayout{m_coloredMeshPipelineLayout},
		  .pipelineCache{m_pipelineCache}};

		m_coloredMeshPipeline = builder.build_pipeline(m_device, m_renderPass);

		m_deletionQueue.push([=]() { m_device.destroyPipeline(m_coloredMeshPipeline); });

		m_device.destroyShaderModule(coloredMeshVertShader);
		m_device.destroyShaderModule(coloredMeshFragShader);
	}

	void VkRenderer::destroySwapchain()
	{
		if (!isInitalized) { return; }

		m_device.destroyRenderPass(m_renderPass);
		for (const auto& framebuffer : m_framebuffers) { m_device.destroyFramebuffer(framebuffer); }
		m_device.destroyCommandPool(m_cmdPool);
		m_device.destroySwapchainKHR(m_swapchain);
		for (const auto& imageView : m_swapchainImageViews) { m_device.destroyImageView(imageView); }
	}
}  // namespace MRG
