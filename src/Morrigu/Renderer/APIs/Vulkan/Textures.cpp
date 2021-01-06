#include "Textures.h"

#include "Debug/Instrumentor.h"
#include "Renderer/ImageLoader.h"
#include "Renderer/Renderer2D.h"

#include <GLFW/glfw3.h>
#include <Vendor/ImGui/bindings/imgui_impl_vulkan.h>
#include <stb_image.h>

namespace MRG::Vulkan
{
	Texture2D::Texture2D(uint32_t width, uint32_t height) : m_width(width), m_height(height)
	{
		MRG_PROFILE_FUNCTION();

		const auto windowData = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		createImage(windowData->physicalDevice,
		            windowData->device,
		            m_width,
		            m_height,
		            VK_FORMAT_R8G8B8A8_UNORM,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_imageHandle,
		            m_memoryHandle);

		transitionImageLayout(windowData, m_imageHandle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_imageView = createImageView(windowData->device, m_imageHandle, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

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

		MRG_VKVALIDATE(vkCreateSampler(windowData->device, &samplerInfo, nullptr, &m_sampler), "failed to create texture sampler!");
	}

	Texture2D::Texture2D(const std::string& path)
	{
		MRG_PROFILE_FUNCTION();

		int width, height, channels;
		const auto pixels = ImageLoader::loadFromFile(path.c_str(), &width, &height, &channels, STBI_rgb_alpha, true);
		MRG_CORE_ASSERT(pixels, fmt::format("Failed to load file '{}'", path));

		m_width = static_cast<uint32_t>(width);
		m_height = static_cast<uint32_t>(height);

		uint32_t imageSize = width * height * 4;

		m_isDestroyed = true;
		setData(pixels, imageSize);
	}

	Texture2D::~Texture2D()
	{
		MRG_PROFILE_FUNCTION();

		destroy();
	}

	void Texture2D::destroy()
	{
		if (m_isDestroyed)
			return;

		const auto windowData = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroySampler(windowData->device, m_sampler, nullptr);
		vkDestroyImageView(windowData->device, m_imageView, nullptr);

		vkDestroyImage(windowData->device, m_imageHandle, nullptr);
		vkFreeMemory(windowData->device, m_memoryHandle, nullptr);

		m_isDestroyed = true;
	}

	ImTextureID Texture2D::getImTextureID()
	{
		if (m_ImTextureID == nullptr) {
			m_ImTextureID = ImGui_ImplVulkan_AddTexture(m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		return m_ImTextureID;
	}

	void Texture2D::setData(void* pixels, uint32_t size)
	{
		MRG_PROFILE_FUNCTION();

		destroy();

		MRG_CORE_ASSERT(size == m_width * m_width * 4, "Data size is incorrect!");

		const auto windowData = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));
		Buffer stagingBuffer;
		createBuffer(windowData->device,
		             windowData->physicalDevice,
		             size,
		             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		             stagingBuffer);

		void* data;
		vkMapMemory(windowData->device, stagingBuffer.memoryHandle, 0, size, 0, &data);
		memcpy(data, pixels, static_cast<std::size_t>(size));
		vkUnmapMemory(windowData->device, stagingBuffer.memoryHandle);

		createImage(windowData->physicalDevice,
		            windowData->device,
		            m_width,
		            m_height,
		            VK_FORMAT_R8G8B8A8_UNORM,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_imageHandle,
		            m_memoryHandle);

		transitionImageLayout(windowData, m_imageHandle, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(windowData, stagingBuffer.handle, m_imageHandle, m_width, m_height);
		transitionImageLayout(windowData, m_imageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(windowData->device, stagingBuffer.handle, nullptr);
		vkFreeMemory(windowData->device, stagingBuffer.memoryHandle, nullptr);

		m_imageView = createImageView(windowData->device, m_imageHandle, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

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

		MRG_VKVALIDATE(vkCreateSampler(windowData->device, &samplerInfo, nullptr, &m_sampler), "failed to create texture sampler!");

		m_isDestroyed = false;
	}

	void Texture2D::bind(uint32_t) const
	{
		// MRG_PROFILE_FUNCTION();
	}
}  // namespace MRG::Vulkan
