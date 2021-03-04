#include "Buffers.h"

#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/Renderer2D.h"

namespace MRG::Vulkan
{
	VertexBuffer::VertexBuffer(uint32_t size)
	{
		MRG_PROFILE_FUNCTION()

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		MRG::Vulkan::createBuffer(data->device,
		                          data->physicalDevice,
		                          size,
		                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                          m_bufferStruct);
	}

	VertexBuffer::VertexBuffer(const void* vertices, uint32_t size)
	{
		MRG_PROFILE_FUNCTION()

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		MRG::Vulkan::Buffer stagingBuffer{};
		MRG::Vulkan::createBuffer(data->device,
		                          data->physicalDevice,
		                          size,
		                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		                          stagingBuffer);

		void* dataPointer;
		vkMapMemory(data->device, stagingBuffer.memoryHandle, 0, size, 0, &dataPointer);
		memcpy(dataPointer, vertices, size);
		vkUnmapMemory(data->device, stagingBuffer.memoryHandle);

		MRG::Vulkan::createBuffer(data->device,
		                          data->physicalDevice,
		                          size,
		                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                          m_bufferStruct);

		MRG::Vulkan::copyBuffer(data, stagingBuffer, m_bufferStruct, size);

		vkDestroyBuffer(data->device, stagingBuffer.handle, nullptr);
		vkFreeMemory(data->device, stagingBuffer.memoryHandle, nullptr);
	}

	VertexBuffer::~VertexBuffer()
	{
		MRG_PROFILE_FUNCTION()

		VertexBuffer::destroy();
	}

	void VertexBuffer::destroy()
	{
		MRG_PROFILE_FUNCTION()

		if (m_isDestroyed) {
			return;
		}

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroyBuffer(data->device, m_bufferStruct.handle, nullptr);
		vkFreeMemory(data->device, m_bufferStruct.memoryHandle, nullptr);
		m_isDestroyed = true;
	}

	void VertexBuffer::bind() const
	{
		// MRG_PROFILE_FUNCTION()
	}
	void VertexBuffer::unbind() const
	{
		// MRG_PROFILE_FUNCTION()
	}

	void VertexBuffer::setData(const void* data, uint32_t size)
	{
		const auto windowData = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		MRG::Vulkan::Buffer stagingBuffer{};
		MRG::Vulkan::createBuffer(windowData->device,
		                          windowData->physicalDevice,
		                          size,
		                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		                          stagingBuffer);

		void* dataPointer;
		vkMapMemory(windowData->device, stagingBuffer.memoryHandle, 0, size, 0, &dataPointer);
		memcpy(dataPointer, data, size);
		vkUnmapMemory(windowData->device, stagingBuffer.memoryHandle);

		MRG::Vulkan::copyBuffer(windowData, stagingBuffer, m_bufferStruct, size);

		vkDestroyBuffer(windowData->device, stagingBuffer.handle, nullptr);
		vkFreeMemory(windowData->device, stagingBuffer.memoryHandle, nullptr);
	}

	//===================================================================================//

	IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count) : m_count(count)
	{
		MRG_PROFILE_FUNCTION()

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));
		const auto bufferSize = count * sizeof(uint32_t);

		MRG::Vulkan::Buffer stagingBuffer{};
		MRG::Vulkan::createBuffer(data->device,
		                          data->physicalDevice,
		                          bufferSize,
		                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		                          stagingBuffer);

		void* dataPointer;
		vkMapMemory(data->device, stagingBuffer.memoryHandle, 0, bufferSize, 0, &dataPointer);
		memcpy(dataPointer, indices, bufferSize);
		vkUnmapMemory(data->device, stagingBuffer.memoryHandle);
		MRG_ENGINE_TRACE("Index buffer data successfully bound and updloaded")

		MRG::Vulkan::createBuffer(data->device,
		                          data->physicalDevice,
		                          bufferSize,
		                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		                          m_bufferStruct);

		copyBuffer(data, stagingBuffer, m_bufferStruct, bufferSize);

		vkDestroyBuffer(data->device, stagingBuffer.handle, nullptr);
		vkFreeMemory(data->device, stagingBuffer.memoryHandle, nullptr);
	}

	IndexBuffer::~IndexBuffer()
	{
		MRG_PROFILE_FUNCTION()

		IndexBuffer::destroy();
	}

	void IndexBuffer::destroy()
	{
		MRG_PROFILE_FUNCTION()

		if (m_isDestroyed) {
			return;
		}

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroyBuffer(data->device, m_bufferStruct.handle, nullptr);
		vkFreeMemory(data->device, m_bufferStruct.memoryHandle, nullptr);
		m_isDestroyed = true;
	}

	void IndexBuffer::bind() const
	{
		// MRG_PROFILE_FUNCTION()
	}
	void IndexBuffer::unbind() const
	{
		// MRG_PROFILE_FUNCTION()
	}
}  // namespace MRG::Vulkan
