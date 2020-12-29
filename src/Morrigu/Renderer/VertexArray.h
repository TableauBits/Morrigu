#ifndef MRG_CLASS_VERTEXARRAY
#define MRG_CLASS_VERTEXARRAY

#include "Core/Core.h"
#include "Renderer/Buffers.h"

namespace MRG
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void destroy() = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		[[nodiscard]] virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;
		[[nodiscard]] virtual const Ref<IndexBuffer>& getIndexBuffer() const = 0;

		[[nodiscard]] static Ref<VertexArray> create();

	protected:
		bool m_isDestroyed = false;
	};
}  // namespace MRG

#endif
