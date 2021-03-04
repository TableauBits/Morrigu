#include "Framebuffer.h"

#include "Renderer/Renderer2D.h"

#include <Vendor/ImGui/bindings/imgui_impl_vulkan.h>

#include <algorithm>

namespace MRG::Vulkan
{
	[[nodiscard]] VkFormat internalToVulkanFormat(MRG::FramebufferTextureFormat format)
	{
		switch (format) {
		case MRG::FramebufferTextureFormat::RGBA8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case MRG::FramebufferTextureFormat::RGBA16:
			return VK_FORMAT_R16G16B16A16_UNORM;

		case MRG::FramebufferTextureFormat::DEPTH24STENCIL8:
			return VK_FORMAT_D24_UNORM_S8_UINT;

		case MRG::FramebufferTextureFormat::None: {
			MRG_CORE_ASSERT(false, "invalid format!")
			return VK_FORMAT_R8G8B8A8_UNORM;
		}
		}
		return VK_FORMAT_R8G8B8A8_UNORM;
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
	{
		auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		m_specification = spec;
		for (const auto& attachment : spec.attachments.attachments) {
			if (!isDepthFormat(attachment.textureFormat)) {
				m_colorAttachmentsSpecifications.emplace_back(attachment);
			} else {
				m_depthAttachmentsSpecification = attachment;
			}
		}

		m_clearValues.resize(spec.attachments.attachments.size());

		std::vector<VkFormat> vulkanFormats{};
		VkFormat depthFormat{};

		for (const auto& attachment : m_specification.attachments.attachments) {
			if (!isDepthFormat(attachment.textureFormat)) {
				vulkanFormats.emplace_back(internalToVulkanFormat(attachment.textureFormat));
			} else {
				depthFormat = internalToVulkanFormat(attachment.textureFormat);
			}
		}

		VkAttachmentDescription colorAttachment{};
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		PipelineSpec pipelineSpec{data->textureShader,
		                          vulkanFormats,
		                          depthFormat,
		                          colorAttachment,
		                          depthAttachment,
		                          std::static_pointer_cast<MRG::Vulkan::VertexArray>(data->vertexArray)->getAttributeDescriptions(),
		                          {std::static_pointer_cast<MRG::Vulkan::VertexArray>(data->vertexArray)->getBindingDescription()}};
		m_clearingPipeline.init(pipelineSpec);

		pipelineSpec.colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		pipelineSpec.depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

		m_renderingPipeline.init(pipelineSpec);

		m_colorAttachments.resize(m_colorAttachmentsSpecifications.size());
		m_ImTextureIDs.resize(m_colorAttachmentsSpecifications.size(), nullptr);

		std::vector<VkImageView> attachments{};

		for (std::size_t i = 0; i < m_colorAttachmentsSpecifications.size(); ++i) {
			createImage(data->physicalDevice,
			            data->device,
			            m_specification.width,
			            m_specification.height,
			            internalToVulkanFormat(m_colorAttachmentsSpecifications[i].textureFormat),
			            VK_IMAGE_TILING_OPTIMAL,
			            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			            m_colorAttachments[i].handle,
			            m_colorAttachments[i].memoryHandle);

			transitionImageLayout(data, m_colorAttachments[i].handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			m_colorAttachments[i].imageView = createImageView(data->device,
			                                                  m_colorAttachments[i].handle,
			                                                  internalToVulkanFormat(m_colorAttachmentsSpecifications[i].textureFormat),
			                                                  VK_IMAGE_ASPECT_COLOR_BIT);

			attachments.emplace_back(m_colorAttachments[i].imageView);
		}

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

		if (m_depthAttachmentsSpecification.textureFormat != FramebufferTextureFormat::None) {
			createImage(data->physicalDevice,
			            data->device,
			            m_specification.width,
			            m_specification.height,
			            internalToVulkanFormat(m_depthAttachmentsSpecification.textureFormat),
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
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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

			m_depthAttachment.imageView = createImageView(data->device,
			                                              m_depthAttachment.handle,
			                                              internalToVulkanFormat(m_depthAttachmentsSpecification.textureFormat),
			                                              VK_IMAGE_ASPECT_DEPTH_BIT);

			attachments.emplace_back(m_depthAttachment.imageView);
		}

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = m_specification.width;
		createInfo.height = m_specification.height;
		createInfo.layers = 1;
		createInfo.renderPass = m_renderingPipeline.getRenderpass();  // We can use either RP, as we define them as compatible
		// RP compatibility is defined here:
		// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#renderpass-compatibility

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!")
	}  // namespace MRG::Vulkan

	Framebuffer::~Framebuffer() { Framebuffer::destroy(); }

	void Framebuffer::destroy()
	{
		if (m_isDestroyed) {
			return;
		}

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroyFramebuffer(data->device, m_handle, nullptr);

		vkDestroySampler(data->device, m_sampler, nullptr);

		for (auto& attachment : m_colorAttachments) {
			vkDestroyImageView(data->device, attachment.imageView, nullptr);
			vkDestroyImage(data->device, attachment.handle, nullptr);
			vkFreeMemory(data->device, attachment.memoryHandle, nullptr);
		}

		if (m_depthAttachmentsSpecification.textureFormat != FramebufferTextureFormat::None) {
			vkDestroyImageView(data->device, m_depthAttachment.imageView, nullptr);
			vkDestroyImage(data->device, m_depthAttachment.handle, nullptr);
			vkFreeMemory(data->device, m_depthAttachment.memoryHandle, nullptr);
		}

		m_clearingPipeline.destroy();
		m_renderingPipeline.destroy();

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

		for (auto& attachment : m_colorAttachments) {
			vkDestroyImageView(data->device, attachment.imageView, nullptr);
			vkDestroyImage(data->device, attachment.handle, nullptr);
			vkFreeMemory(data->device, attachment.memoryHandle, nullptr);
		}

		if (m_depthAttachmentsSpecification.textureFormat != FramebufferTextureFormat::None) {
			vkDestroyImageView(data->device, m_depthAttachment.imageView, nullptr);
			vkDestroyImage(data->device, m_depthAttachment.handle, nullptr);
			vkFreeMemory(data->device, m_depthAttachment.memoryHandle, nullptr);
		}

		vkDestroyFramebuffer(data->device, m_handle, nullptr);

		std::vector<VkImageView> attachments;

		for (std::size_t i = 0; i < m_colorAttachmentsSpecifications.size(); ++i) {
			const auto format = internalToVulkanFormat(m_colorAttachmentsSpecifications[i].textureFormat);
			createImage(data->physicalDevice,
			            data->device,
			            m_specification.width,
			            m_specification.height,
			            format,
			            VK_IMAGE_TILING_OPTIMAL,
			            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			            m_colorAttachments[i].handle,
			            m_colorAttachments[i].memoryHandle);

			transitionImageLayout(data, m_colorAttachments[i].handle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			m_colorAttachments[i].imageView =
			  createImageView(data->device, m_colorAttachments[i].handle, format, VK_IMAGE_ASPECT_COLOR_BIT);

			attachments.emplace_back(m_colorAttachments[i].imageView);
		}

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

		if (m_depthAttachmentsSpecification.textureFormat != FramebufferTextureFormat::None) {
			createImage(data->physicalDevice,
			            data->device,
			            m_specification.width,
			            m_specification.height,
			            internalToVulkanFormat(m_depthAttachmentsSpecification.textureFormat),
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
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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

			m_depthAttachment.imageView = createImageView(data->device,
			                                              m_depthAttachment.handle,
			                                              internalToVulkanFormat(m_depthAttachmentsSpecification.textureFormat),
			                                              VK_IMAGE_ASPECT_DEPTH_BIT);

			attachments.emplace_back(m_depthAttachment.imageView);
		}

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = m_specification.width;
		createInfo.height = m_specification.height;
		createInfo.layers = 1;
		createInfo.renderPass = m_renderingPipeline.getRenderpass();

		MRG_VKVALIDATE(vkCreateFramebuffer(data->device, &createInfo, nullptr, &m_handle), "failed to create framebuffer!")

		for (std::size_t i = 0; i < m_colorAttachments.size(); ++i) {
			if (m_ImTextureIDs[i] != nullptr) {
				transitionImageLayout(
				  data, m_colorAttachments[i].handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				VkDescriptorImageInfo desc_image{};
				desc_image.sampler = m_sampler;
				desc_image.imageView = m_colorAttachments[i].imageView;
				desc_image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				VkWriteDescriptorSet write_desc{};
				write_desc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write_desc.dstSet = (VkDescriptorSet)m_ImTextureIDs[i];
				write_desc.descriptorCount = 1;
				write_desc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				write_desc.pImageInfo = &desc_image;
				vkUpdateDescriptorSets(data->device, 1, &write_desc, 0, nullptr);
				transitionImageLayout(
				  data, m_colorAttachments[i].handle, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			}
		}
	}

	ImTextureID Framebuffer::getImTextureID(uint32_t index)
	{
		if (m_ImTextureIDs[index] == nullptr) {
			const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

			transitionImageLayout(
			  data, m_colorAttachments[index].handle, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_ImTextureIDs[index] =
			  ImGui_ImplVulkan_AddTexture(m_sampler, m_colorAttachments[index].imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			transitionImageLayout(
			  data, m_colorAttachments[index].handle, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		return m_ImTextureIDs[index];
	}

	void Framebuffer::setClearColor(const glm::vec4& color)
	{
		for (auto& clearValue : m_clearValues) { clearValue.color = {{color.r, color.g, color.b, color.a}}; }
		m_clearValues.back().depthStencil = {1.f, 0};
	}
}  // namespace MRG::Vulkan
