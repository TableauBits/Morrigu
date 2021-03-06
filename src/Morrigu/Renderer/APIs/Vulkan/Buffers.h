#ifndef MRG_VULKAN_IMPL_BUFFERS
#define MRG_VULKAN_IMPL_BUFFERS

#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"
#include "Renderer/Buffers.h"

namespace MRG::Vulkan
{
	class VertexBuffer : public MRG::VertexBuffer
	{
	public:
		explicit VertexBuffer(uint32_t size);
		VertexBuffer(const void* vertices, uint32_t size);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;
		~VertexBuffer() override;

		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) = delete;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

		void setData(const void* data, uint32_t size) override;

		[[nodiscard]] VkBuffer getHandle() const { return m_bufferStruct.handle; }
		[[nodiscard]] VkDeviceMemory getMemoryHandle() const { return m_bufferStruct.memoryHandle; }

	private:
		Buffer m_bufferStruct{};
	};

	class IndexBuffer : public MRG::IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&&) = delete;
		~IndexBuffer() override;

		IndexBuffer& operator=(const IndexBuffer&) = delete;
		IndexBuffer& operator=(IndexBuffer&&) = delete;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

		[[nodiscard]] VkBuffer getHandle() const { return m_bufferStruct.handle; }
		[[nodiscard]] VkDeviceMemory getMemoryHandle() const { return m_bufferStruct.memoryHandle; }

		[[nodiscard]] uint32_t getCount() const override { return m_count; };

	private:
		Buffer m_bufferStruct{};
		uint32_t m_count;
	};
}  // namespace MRG::Vulkan

#endif
