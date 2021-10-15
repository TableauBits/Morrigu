#include "Framebuffer.h"

#include <Vendor/ImGui/bindings/imgui_impl_vulkan.h>

namespace MRG
{
	Framebuffer::Framebuffer(const FramebufferSpecification& specification, const VulkanObjects vkObjs)
	    : spec{specification}, m_objects{vkObjs}
	{
		invalidate();

		const vk::SamplerCreateInfo samplerInfo{
		  .magFilter    = spec.samplingFilter,
		  .minFilter    = spec.samplingFilter,
		  .addressModeU = spec.samplingAddressMode,
		  .addressModeV = spec.samplingAddressMode,
		  .addressModeW = spec.samplingAddressMode,
		};
		sampler = m_objects.device.createSampler(samplerInfo);
	}

	Framebuffer::~Framebuffer()
	{
		m_objects.device.destroySampler(sampler);

		m_objects.device.destroyDescriptorPool(descriptorPool);
		m_objects.device.destroyFence(renderFence);
		m_objects.device.destroyCommandPool(commandPool);
	}

	void Framebuffer::resize(uint32_t width, uint32_t height)
	{
		spec.width  = width;
		spec.height = height;
		invalidate();
	}

	void Framebuffer::invalidate()
	{
		if (descriptorPool != vk::DescriptorPool{}) { m_objects.device.destroyDescriptorPool(descriptorPool); }
		if (renderFence != vk::Fence{}) { m_objects.device.destroyFence(renderFence); }
		if (commandPool != vk::CommandPool{}) { m_objects.device.destroyCommandPool(commandPool); }
		if (vkHandle != vk::Framebuffer{}) { m_objects.device.destroyFramebuffer(vkHandle); }

		colorImage = AllocatedImage{AllocatedImageSpecification{
		  .device        = m_objects.device,
		  .graphicsQueue = m_objects.graphicsQueue,
		  .uploadContext = m_objects.uploadContext,
		  .allocator     = m_objects.allocator,
		  .usage  = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		  .format = m_objects.swapchainFormat,
		  .width  = spec.width,
		  .height = spec.height,
		}};  // namespace MRG

		AllocatedImage tempDepthImage{};

		tempDepthImage.spec.device = m_objects.device;
		tempDepthImage.spec.format = m_objects.depthImageFormat;
		vk::Extent3D depthImageExtent{
		  .width  = spec.width,
		  .height = spec.height,
		  .depth  = 1,
		};
		VkImageCreateInfo depthImageCreateInfo{
		  .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		  .pNext                 = nullptr,
		  .flags                 = 0,
		  .imageType             = VK_IMAGE_TYPE_2D,
		  .format                = static_cast<VkFormat>(tempDepthImage.spec.format),
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
		vmaCreateImage(
		  m_objects.allocator, &depthImageCreateInfo, &depthImageAllocationCreateInfo, &rawImage, &tempDepthImage.allocation, nullptr);
		tempDepthImage.vkHandle = rawImage;

		vk::ImageViewCreateInfo depthImageViewCreateInfo{
		  .image    = tempDepthImage.vkHandle,
		  .viewType = vk::ImageViewType::e2D,
		  .format   = tempDepthImage.spec.format,
		  .subresourceRange{
		    .aspectMask     = vk::ImageAspectFlagBits::eDepth,
		    .baseMipLevel   = 0,
		    .levelCount     = 1,
		    .baseArrayLayer = 0,
		    .layerCount     = 1,
		  },
		};
		tempDepthImage.view = m_objects.device.createImageView(depthImageViewCreateInfo);

		// To make the (potentially) old depth image go out of scope:
		depthImage = std::move(tempDepthImage);

		std::array<vk::ImageView, 2> attachments{colorImage.view, depthImage.view};

		vk::FramebufferCreateInfo fbInfo{
		  // By locking the attachments of the framebuffer, we should guarantee compatibility
		  // (https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap8.html#renderpass-compatibility)
		  .renderPass      = m_objects.renderPass,
		  .attachmentCount = static_cast<uint32_t>(attachments.size()),
		  .pAttachments    = attachments.data(),
		  .width           = spec.width,
		  .height          = spec.height,
		  .layers          = 1,
		};

		vkHandle = m_objects.device.createFramebuffer(fbInfo);

		vk::CommandPoolCreateInfo cmdPoolInfo{
		  .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		  .queueFamilyIndex = m_objects.graphicsQueueIndex,
		};
		commandPool = m_objects.device.createCommandPool(cmdPoolInfo);

		// allocate main command buffer from created command pool
		vk::CommandBufferAllocateInfo mainCmdBufferInfo{
		  .commandPool        = commandPool,
		  .level              = vk::CommandBufferLevel::ePrimary,
		  .commandBufferCount = 1,
		};
		commandBuffer = m_objects.device.allocateCommandBuffers(mainCmdBufferInfo)[0];

		renderFence = m_objects.device.createFence(vk::FenceCreateInfo{});

		std::array<vk::DescriptorPoolSize, 1> sizes{{vk::DescriptorType::eUniformBuffer, 1}};
		vk::DescriptorPoolCreateInfo poolInfo{
		  .maxSets       = 1,
		  .poolSizeCount = static_cast<uint32_t>(sizes.size()),
		  .pPoolSizes    = sizes.data(),
		};
		descriptorPool = m_objects.device.createDescriptorPool(poolInfo);

		vk::DescriptorSetAllocateInfo allocInfo{
		  .descriptorPool     = descriptorPool,
		  .descriptorSetCount = 1,
		  .pSetLayouts        = &m_objects.level0DSL,
		};
		level0Descriptor = m_objects.device.allocateDescriptorSets(allocInfo)[0];

		timeDataBuffer =
		  AllocatedBuffer{m_objects.allocator, sizeof(TimeData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU};

		if (m_imTexID != nullptr) {
			vk::DescriptorImageInfo descImage{
			  .sampler     = sampler,
			  .imageView   = colorImage.view,
			  .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			};
			vk::WriteDescriptorSet dsWrite{
			  .dstSet          = reinterpret_cast<VkDescriptorSet>(m_imTexID),
			  .descriptorCount = 1,
			  .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
			  .pImageInfo      = &descImage,
			};
			m_objects.device.updateDescriptorSets(dsWrite, {});
		}
	}

	ImTextureID Framebuffer::getImTexID()
	{
		if (m_imTexID == nullptr) {
			m_imTexID = ImGui_ImplVulkan_AddTexture(sampler, colorImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		return m_imTexID;
	}
}  // namespace MRG
