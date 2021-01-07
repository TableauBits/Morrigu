#ifndef MRG_OPENGL_IMPL_VERTEXARRAY
#define MRG_OPENGL_IMPL_VERTEXARRAY

#include "Renderer/VertexArray.h"

namespace MRG::OpenGL
{
	class VertexArray : public MRG::VertexArray
	{
	public:
		VertexArray();
		~VertexArray() override;

		void destroy() override
		{
			if (m_isDestroyed)
				return;

			for (auto& vb : m_vertexBuffers) vb->destroy();
			m_indexBuffer->destroy();
			m_isDestroyed = true;
		}

		void bind() const override;
		void unbind() const override;

		void addVertexBuffer(const Ref<MRG::VertexBuffer>& vertexBuffer) override;
		void setIndexBuffer(const Ref<MRG::IndexBuffer>& indexBuffer) override;

		[[nodiscard]] const std::vector<Ref<MRG::VertexBuffer>>& getVertexBuffers() const override { return m_vertexBuffers; };
		[[nodiscard]] const Ref<MRG::IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; };

	private:
		uint32_t m_rendererID{0};
		std::vector<Ref<MRG::VertexBuffer>> m_vertexBuffers;
		Ref<MRG::IndexBuffer> m_indexBuffer;
	};
}  // namespace MRG::OpenGL

#endif
