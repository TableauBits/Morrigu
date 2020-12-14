#include "Framebuffer.h"

#include "Renderer/Renderer2D.h"

#include <Vendor/ImGui/bindings/imgui_impl_vulkan.h>

#include <array>

namespace MRG::Vulkan
{
	Framebuffer::Framebuffer(const FramebufferSpecification& spec) : m_renderTexture(spec.width, spec.height)
	{
		auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		m_specification = spec;

		createImage(data->physicalDevice,
		            data->device,
		            data->swapChain.extent.width,
		            data->swapChain.extent.height,
		            data->swapChain.imageFormat,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_colorAttachment.handle,
		            m_colorAttachment.memoryHandle);

		transitionImageLayout(data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_colorAttachment.imageView =
		  createImageView(data->device, m_colorAttachment.handle, data->swapChain.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16.f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.f;
		samplerInfo.minLod = 0.f;
		samplerInfo.maxLod = 0.f;

		MRG_VKVALIDATE(vkCreateSampler(data->device, &samplerInfo, nullptr, &m_sampler), "failed to create texture sampler!");

		createImage(data->physicalDevice,
		            data->device,
		            data->swapChain.extent.width,
		            data->swapChain.extent.height,
		            VK_FORMAT_D32_SFLOAT,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_depthAttachment.handle,
		            m_depthAttachment.memoryHandle);

		const auto commandBuffer = beginSingleTimeCommand(data);

		VkPipelineStageFlags sourceStage, destinationStage;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_depthAttachment.handle;
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

		m_depthAttachment.imageView =
		  createImageView(data->device, m_depthAttachment.handle, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

		std::array<VkImageView, 2> attachments{m_colorAttachment.imageView, m_depthAttachment.imageView};

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = data->swapChain.extent.width;
		createInfo.height = data->swapChain.extent.height;
		createInfo.layers = 1;
		createInfo.renderPass = data->renderingPipeline.renderPass;

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!");

		m_FBWidth = data->swapChain.extent.width;
		m_FBHeight = data->swapChain.extent.height;
	}

	Framebuffer::~Framebuffer() { destroy(); }

	void Framebuffer::destroy()
	{
		if (m_isDestroyed)
			return;

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroySampler(data->device, m_sampler, nullptr);

		vkDestroyImageView(data->device, m_colorAttachment.imageView, nullptr);
		vkDestroyImageView(data->device, m_depthAttachment.imageView, nullptr);

		vkDestroyImage(data->device, m_colorAttachment.handle, nullptr);
		vkDestroyImage(data->device, m_depthAttachment.handle, nullptr);

		vkFreeMemory(data->device, m_colorAttachment.memoryHandle, nullptr);
		vkFreeMemory(data->device, m_depthAttachment.memoryHandle, nullptr);

		vkDestroyFramebuffer(data->device, m_handle, nullptr);

		m_renderTexture.destroy();

		m_isDestroyed = true;
	}

	void Framebuffer::invalidate()
	{
		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroyImageView(data->device, m_colorAttachment.imageView, nullptr);
		vkDestroyImageView(data->device, m_depthAttachment.imageView, nullptr);

		vkDestroyImage(data->device, m_colorAttachment.handle, nullptr);
		vkDestroyImage(data->device, m_depthAttachment.handle, nullptr);

		vkFreeMemory(data->device, m_colorAttachment.memoryHandle, nullptr);
		vkFreeMemory(data->device, m_depthAttachment.memoryHandle, nullptr);

		vkDestroyFramebuffer(data->device, m_handle, nullptr);

		createImage(data->physicalDevice,
		            data->device,
		            data->swapChain.extent.width,
		            data->swapChain.extent.height,
		            data->swapChain.imageFormat,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_colorAttachment.handle,
		            m_colorAttachment.memoryHandle);

		transitionImageLayout(data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_colorAttachment.imageView =
		  createImageView(data->device, m_colorAttachment.handle, data->swapChain.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		createImage(data->physicalDevice,
		            data->device,
		            data->swapChain.extent.width,
		            data->swapChain.extent.height,
		            VK_FORMAT_D32_SFLOAT,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_depthAttachment.handle,
		            m_depthAttachment.memoryHandle);

		const auto commandBuffer = beginSingleTimeCommand(data);

		VkPipelineStageFlags sourceStage, destinationStage;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_depthAttachment.handle;
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

		m_depthAttachment.imageView =
		  createImageView(data->device, m_depthAttachment.handle, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

		std::array<VkImageView, 2> attachments{m_colorAttachment.imageView, m_depthAttachment.imageView};

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = data->swapChain.extent.width;
		createInfo.height = data->swapChain.extent.height;
		createInfo.layers = 1;
		createInfo.renderPass = data->renderingPipeline.renderPass;

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!");
	}

	ImTextureID Framebuffer::getImTextureID()
	{
		if (m_ImTextureID == nullptr) {
			m_ImTextureID =
			  ImGui_ImplVulkan_AddTexture(m_sampler, m_renderTexture.getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		return m_ImTextureID;
	}

	void Framebuffer::updateView(VkCommandBuffer commandBuffer, bool isClearCommand)
	{
		auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		transitionImageLayoutInline(
		  commandBuffer, m_colorAttachment.handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		transitionImageLayoutInline(
		  commandBuffer, m_renderTexture.getHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkOffset3D offset = {static_cast<int32_t>(std::min(data->swapChain.extent.width, m_specification.width)),
		                     static_cast<int32_t>(std::min(data->swapChain.extent.height, m_specification.height)),
		                     1};

		VkImageBlit region{};
		region.srcOffsets[0] = {0, 0, 0};
		region.srcOffsets[1] = offset;
		region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.srcSubresource.mipLevel = 0;
		region.srcSubresource.baseArrayLayer = 0;
		region.srcSubresource.layerCount = 1;

		region.dstOffsets[0] = {0, 0, 0};
		region.dstOffsets[1] = offset;
		region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.dstSubresource.mipLevel = 0;
		region.dstSubresource.baseArrayLayer = 0;
		region.dstSubresource.layerCount = 1;

		if (isClearCommand) {
			region.srcOffsets[1] = {
			  static_cast<int32_t>(data->swapChain.extent.width), static_cast<int32_t>(data->swapChain.extent.height), 1};
			region.dstOffsets[1] = {static_cast<int32_t>(m_specification.width), static_cast<int32_t>(m_specification.height), 1};
		}

		vkCmdBlitImage(commandBuffer,
		               m_colorAttachment.handle,
		               VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		               m_renderTexture.getHandle(),
		               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		               1,
		               &region,
		               VK_FILTER_LINEAR);

		transitionImageLayoutInline(
		  commandBuffer, m_renderTexture.getHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		transitionImageLayoutInline(
		  commandBuffer, m_colorAttachment.handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
}  // namespace MRG::Vulkan
