#ifndef MRG_CLASS_VERTEXARRAY
#define MRG_CLASS_VERTEXARRAY

#include "Renderer/Buffers.h"

namespace MRG
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		[[nodiscard]] virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const = 0;
		[[nodiscard]] virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const = 0;

		[[nodiscard]] static VertexArray* create();
	};
}  // namespace MRG

#endif