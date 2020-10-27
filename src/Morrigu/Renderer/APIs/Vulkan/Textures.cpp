#include "Textures.h"

#include "Debug/Instrumentor.h"
#include "Renderer/ImageLoader.h"
#include "Renderer/Renderer2D.h"

#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace MRG::Vulkan
{
	Texture2D::Texture2D(uint32_t width, uint32_t height) : m_width(width), m_height(height)
	{
		// MRG_PROFILE_FUNCTION();
	}

	Texture2D::Texture2D(const std::string& path)
	{
		MRG_PROFILE_FUNCTION();

		int width, height, channels;
		const auto pixels = ImageLoader::loadFromFile(path.c_str(), &width, &height, &channels, 0, false);
		MRG_CORE_ASSERT(pixels, fmt::format("Failed to load file '{}'", path));

		m_width = static_cast<uint32_t>(width);
		m_height = static_cast<uint32_t>(height);

		VkDeviceSize imageSize = width * height * 4;

		const auto windowData = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));
		Buffer stagingBuffer;
		createBuffer(windowData->device,
		             windowData->physicalDevice,
		             imageSize,
		             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		             stagingBuffer);

		void* data;
		vkMapMemory(windowData->device, stagingBuffer.memoryHandle, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<std::size_t>(imageSize));
		vkUnmapMemory(windowData->device, stagingBuffer.memoryHandle);

		stbi_image_free(pixels);

		createImage(windowData->physicalDevice,
		            windowData->device,
		            m_width,
		            m_height,
		            VK_FORMAT_R8G8B8A8_SRGB,
		            VK_IMAGE_TILING_OPTIMAL,
		            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		            m_imageHandle,
		            m_memoryHandle);

		transitionImageLayout(
		  windowData, m_imageHandle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(windowData, stagingBuffer.handle, m_imageHandle, m_width, m_height);
		transitionImageLayout(windowData,
		                      m_imageHandle,
		                      VK_FORMAT_R8G8B8A8_SRGB,
		                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(windowData->device, stagingBuffer.handle, nullptr);
		vkFreeMemory(windowData->device, stagingBuffer.memoryHandle, nullptr);
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

		vkDestroyImage(windowData->device, m_imageHandle, nullptr);
		vkFreeMemory(windowData->device, m_memoryHandle, nullptr);

		m_isDestroyed = true;
	}

	void Texture2D::setData(void*, uint32_t)
	{
		// MRG_PROFILE_FUNCTION();
	}

	void Texture2D::bind(uint32_t) const
	{
		// MRG_PROFILE_FUNCTION();
	}
}  // namespace MRG::Vulkan