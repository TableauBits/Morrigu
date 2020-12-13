#include "Framebuffer.h"

#include "Renderer/Renderer2D.h"

#include <Vendor/ImGui/bindings/imgui_impl_vulkan.h>

#include <array>

namespace MRG::Vulkan
{
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
	{
		auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		m_specification = spec;

		createImage(data->physicalDevice,
		            data->device,
		            m_specification.width,
		            m_specification.height,
		            VK_FORMAT_B8G8R8A8_UNORM,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_colorAttachment.handle,
		            m_colorAttachment.memoryHandle);

		transitionImageLayout(
		  data, m_colorAttachment.handle, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_colorAttachment.imageView =
		  createImageView(data->device, m_colorAttachment.handle, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

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
		            m_specification.width,
		            m_specification.height,
		            VK_FORMAT_D32_SFLOAT,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_depthAttachment.handle,
		            m_depthAttachment.memoryHandle);

		const auto commandBuffer = beginSingleTimeCommand(data);

		VkPipelineStageFlags sourceStage, destinationStage;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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
		createInfo.width = m_specification.width;
		createInfo.height = m_specification.height;
		createInfo.layers = 1;
		createInfo.renderPass = data->renderingPipeline.renderPass;

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!");
	}

	Framebuffer::~Framebuffer() { destroy(); }

	ImTextureID Framebuffer::getImTextureID()
	{
		if (m_ImTextureID == nullptr) {
			m_ImTextureID = ImGui_ImplVulkan_AddTexture(m_sampler, m_colorAttachment.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		return m_ImTextureID;
	}

	void Framebuffer::destroy()
	{
		if (m_isDestroyed)
			return;

		const auto windowData = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroySampler(windowData->device, m_sampler, nullptr);

		vkDestroyImageView(windowData->device, m_colorAttachment.imageView, nullptr);
		vkDestroyImageView(windowData->device, m_depthAttachment.imageView, nullptr);

		vkDestroyImage(windowData->device, m_colorAttachment.handle, nullptr);
		vkDestroyImage(windowData->device, m_depthAttachment.handle, nullptr);

		vkFreeMemory(windowData->device, m_colorAttachment.memoryHandle, nullptr);
		vkFreeMemory(windowData->device, m_depthAttachment.memoryHandle, nullptr);

		vkDestroyFramebuffer(windowData->device, m_handle, nullptr);

		m_isDestroyed = true;
	}
}  // namespace MRG::Vulkan
