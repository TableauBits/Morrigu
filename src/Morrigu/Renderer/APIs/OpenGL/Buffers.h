#ifndef MRG_OPENGL_IMPL_BUFFERS
#define MRG_OPENGL_IMPL_BUFFERS

#include "Renderer/Buffers.h"

namespace MRG::OpenGL
{
	class VertexBuffer : public MRG::VertexBuffer
	{
	public:
		explicit VertexBuffer(uint32_t size);
		VertexBuffer(const void* vertices, uint32_t size);
		~VertexBuffer() override;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

		void setData(const void* data, uint32_t size) override;

	private:
		uint32_t m_rendererID{0};
	};

	class IndexBuffer : public MRG::IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);
		~IndexBuffer() override;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

		[[nodiscard]] uint32_t getCount() const override { return m_count; };

	private:
		uint32_t m_rendererID{0};
		uint32_t m_count;
	};
}  // namespace MRG::OpenGL

#endif
