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
		            data->swapChain.imageFormat,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_colorAttachment.handle,
		            m_colorAttachment.memoryHandle);

		transitionImageLayout(data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_colorAttachment.imageView =
		  createImageView(data->device, m_colorAttachment.handle, data->swapChain.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		createImage(data->physicalDevice,
		            data->device,
		            m_specification.width,
		            m_specification.height,
		            VK_FORMAT_R16G16B16A16_UNORM,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_objectIDBuffer.handle,
		            m_objectIDBuffer.memoryHandle);

		transitionImageLayout(data, m_objectIDBuffer.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_objectIDBuffer.imageView =
		  createImageView(data->device, m_objectIDBuffer.handle, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

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

		MRG_VKVALIDATE(vkCreateSampler(data->device, &samplerInfo, nullptr, &m_sampler), "failed to create texture sampler!")

		createImage(data->physicalDevice,
		            data->device,
		            m_specification.width,
		            m_specification.height,
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

		std::array<VkImageView, 3> attachments{m_colorAttachment.imageView, m_objectIDBuffer.imageView, m_depthAttachment.imageView};

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = m_specification.width;
		createInfo.height = m_specification.height;
		createInfo.layers = 1;
		createInfo.renderPass = data->renderingPipeline.renderPass;

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!")

		createBuffer(data->device,
		             data->physicalDevice,
		             data->width * data->height * 8,  // TODO: make this work for something else than 64bits pixel data
		             VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
		             m_objectIDLocalBuffer);
	}

	Framebuffer::~Framebuffer() { Framebuffer::destroy(); }

	void Framebuffer::destroy()
	{
		if (m_isDestroyed)
			return;

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroySampler(data->device, m_sampler, nullptr);

		vkDestroyImageView(data->device, m_colorAttachment.imageView, nullptr);
		vkDestroyImageView(data->device, m_objectIDBuffer.imageView, nullptr);
		vkDestroyImageView(data->device, m_depthAttachment.imageView, nullptr);

		vkDestroyImage(data->device, m_colorAttachment.handle, nullptr);
		vkDestroyImage(data->device, m_objectIDBuffer.handle, nullptr);
		vkDestroyImage(data->device, m_depthAttachment.handle, nullptr);

		vkFreeMemory(data->device, m_colorAttachment.memoryHandle, nullptr);
		vkFreeMemory(data->device, m_objectIDBuffer.memoryHandle, nullptr);
		vkFreeMemory(data->device, m_depthAttachment.memoryHandle, nullptr);

		vkDestroyFramebuffer(data->device, m_handle, nullptr);

		vkDestroyBuffer(data->device, m_objectIDLocalBuffer.handle, nullptr);
		vkFreeMemory(data->device, m_objectIDLocalBuffer.memoryHandle, nullptr);

		m_isDestroyed = true;
	}

	void Framebuffer::resize(uint32_t width, uint32_t height)
	{
		m_specification.width = width;
		m_specification.height = height;

		invalidate();
	}

	void Framebuffer::invalidate()
	{
		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDeviceWaitIdle(data->device);

		vkDestroyImageView(data->device, m_colorAttachment.imageView, nullptr);
		vkDestroyImageView(data->device, m_objectIDBuffer.imageView, nullptr);
		vkDestroyImageView(data->device, m_depthAttachment.imageView, nullptr);

		vkDestroyImage(data->device, m_colorAttachment.handle, nullptr);
		vkDestroyImage(data->device, m_objectIDBuffer.handle, nullptr);
		vkDestroyImage(data->device, m_depthAttachment.handle, nullptr);

		vkFreeMemory(data->device, m_colorAttachment.memoryHandle, nullptr);
		vkFreeMemory(data->device, m_objectIDBuffer.memoryHandle, nullptr);
		vkFreeMemory(data->device, m_depthAttachment.memoryHandle, nullptr);

		vkDestroyFramebuffer(data->device, m_handle, nullptr);

		vkDestroyBuffer(data->device, m_objectIDLocalBuffer.handle, nullptr);
		vkFreeMemory(data->device, m_objectIDLocalBuffer.memoryHandle, nullptr);

		createImage(data->physicalDevice,
		            data->device,
		            m_specification.width,
		            m_specification.height,
		            data->swapChain.imageFormat,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_colorAttachment.handle,
		            m_colorAttachment.memoryHandle);

		transitionImageLayout(data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_colorAttachment.imageView =
		  createImageView(data->device, m_colorAttachment.handle, data->swapChain.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		createImage(data->physicalDevice,
		            data->device,
		            m_specification.width,
		            m_specification.height,
		            VK_FORMAT_R16G16B16A16_UNORM,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_objectIDBuffer.handle,
		            m_objectIDBuffer.memoryHandle);

		transitionImageLayout(data, m_objectIDBuffer.handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		m_objectIDBuffer.imageView =
		  createImageView(data->device, m_objectIDBuffer.handle, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		createImage(data->physicalDevice,
		            data->device,
		            m_specification.width,
		            m_specification.height,
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

		std::array<VkImageView, 3> attachments{m_colorAttachment.imageView, m_objectIDBuffer.imageView, m_depthAttachment.imageView};

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = m_specification.width;
		createInfo.height = m_specification.height;
		createInfo.layers = 1;
		createInfo.renderPass = data->renderingPipeline.renderPass;

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!")

		createBuffer(data->device,
		             data->physicalDevice,
		             data->width * data->height * 8,  // TODO: make this work for something else than 64bits pixel data
		             VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
		             m_objectIDLocalBuffer);

		if (m_ImTextureID != nullptr) {
			transitionImageLayout(
			  data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			VkDescriptorImageInfo desc_image[1] = {};
			desc_image[0].sampler = m_sampler;
			desc_image[0].imageView = m_colorAttachment.imageView;
			desc_image[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			VkWriteDescriptorSet write_desc[1] = {};
			write_desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write_desc[0].dstSet = (VkDescriptorSet)m_ImTextureID;
			write_desc[0].descriptorCount = 1;
			write_desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write_desc[0].pImageInfo = desc_image;
			vkUpdateDescriptorSets(data->device, 1, write_desc, 0, nullptr);
			transitionImageLayout(
			  data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
	}

	ImTextureID Framebuffer::getImTextureID()
	{
		if (m_ImTextureID == nullptr) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

			transitionImageLayout(
			  data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_ImTextureID = ImGui_ImplVulkan_AddTexture(m_sampler, m_colorAttachment.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			transitionImageLayout(
			  data, m_colorAttachment.handle, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		return m_ImTextureID;
	}
}  // namespace MRG::Vulkan
