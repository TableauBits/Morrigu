#ifndef MRG_VULKAN_IMPL_BUFFERS
#define MRG_VULKAN_IMPL_BUFFERS

#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"
#include "Renderer/Buffers.h"

#include <vulkan/vulkan.hpp>

namespace MRG::Vulkan
{
	class VertexBuffer : public MRG::VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, uint32_t size);
		~VertexBuffer() override;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

		[[nodiscard]] VkBuffer getHandle() const { return m_bufferStruct.handle; }
		[[nodiscard]] VkDeviceMemory getMemoryHandle() const { return m_bufferStruct.memoryHandle; }

	private:
		Buffer m_bufferStruct;
	};

	class IndexBuffer : public MRG::IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);
		~IndexBuffer() override;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

		[[nodiscard]] VkBuffer getHandle() const { return m_bufferStruct.handle; }
		[[nodiscard]] VkDeviceMemory getMemoryHandle() const { return m_bufferStruct.memoryHandle; }

		[[nodiscard]] uint32_t getCount() const override { return m_count; };

	private:
		Buffer m_bufferStruct;
		uint32_t m_count;
	};
}  // namespace MRG::Vulkan

#endif