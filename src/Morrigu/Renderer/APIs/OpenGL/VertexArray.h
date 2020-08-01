#ifndef MRG_OPENGL_IMPL_VERTEXARRAY
#define MRG_OPENGL_IMPL_VERTEXARRAY

#include "Renderer/VertexArray.h"

namespace MRG::OpenGL
{
	class VertexArray : public MRG::VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		void bind() const override;
		void unbind() const override;

		void addVertexBuffer(const std::shared_ptr<MRG::VertexBuffer>& vertexBuffer) override;
		void setIndexBuffer(const std::shared_ptr<MRG::IndexBuffer>& indexBuffer) override;

		[[nodiscard]] const std::vector<std::shared_ptr<MRG::VertexBuffer>>& getVertexBuffers() const override { return m_vertexBuffers; };
		[[nodiscard]] const std::shared_ptr<MRG::IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; };

	private:
		uint32_t m_rendererID;
		std::vector<std::shared_ptr<MRG::VertexBuffer>> m_vertexBuffers;
		std::shared_ptr<MRG::IndexBuffer> m_indexBuffer;
	};
}  // namespace MRG::OpenGL

#endif