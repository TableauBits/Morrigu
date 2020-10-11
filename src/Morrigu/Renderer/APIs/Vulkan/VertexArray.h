#ifndef MRG_VULKAN_IMPL_VERTEXARRAY
#define MRG_VULKAN_IMPL_VERTEXARRAY

#include "Renderer/VertexArray.h"

namespace MRG::Vulkan
{
	class VertexArray : public MRG::VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		void destroy() override
		{
			for (auto& vb : m_vertexBuffers) vb->destroy();
			m_indexBuffer->destroy();
		}

		void bind() const override;
		void unbind() const override;

		void addVertexBuffer(const Ref<MRG::VertexBuffer>& vertexBuffer) override;
		void setIndexBuffer(const Ref<MRG::IndexBuffer>& indexBuffer) override;

		[[nodiscard]] const std::vector<Ref<MRG::VertexBuffer>>& getVertexBuffers() const override { return m_vertexBuffers; };
		[[nodiscard]] const Ref<MRG::IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; };

	private:
		// uint32_t m_rendererID;
		std::vector<Ref<MRG::VertexBuffer>> m_vertexBuffers;
		Ref<MRG::IndexBuffer> m_indexBuffer;
	};
}  // namespace MRG::Vulkan

#endif