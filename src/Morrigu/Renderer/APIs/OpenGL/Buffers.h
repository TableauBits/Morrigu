#ifndef MRG_OPENGL_IMPL_BUFFERS
#define MRG_OPENGL_IMPL_BUFFERS

#include "Renderer/Buffers.h"

namespace MRG::OpenGL
{
	class VertexBuffer : public MRG::VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, uint32_t size);
		~VertexBuffer() override;

		void destroy() override;

		void bind() const override;
		void unbind() const override;

	private:
		uint32_t m_rendererID;
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
		uint32_t m_rendererID;
		uint32_t m_count;
	};
}  // namespace MRG::OpenGL

#endif