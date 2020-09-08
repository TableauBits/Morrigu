#include "Buffers.h"

#include "Debug/Instrumentor.h"

namespace MRG::Vulkan
{
	VertexBuffer::VertexBuffer(const float*, uint32_t)
	{
		// MRG_PROFILE_FUNCTION();
	}

	VertexBuffer::~VertexBuffer()
	{
		// MRG_PROFILE_FUNCTION();
	}

	void VertexBuffer::bind() const
	{
		// MRG_PROFILE_FUNCTION();
	}
	void VertexBuffer::unbind() const
	{
		// MRG_PROFILE_FUNCTION();
	}

	//===================================================================================//

	IndexBuffer::IndexBuffer(const uint32_t*, uint32_t count) : m_count(count)
	{
		// MRG_PROFILE_FUNCTION();
	}

	IndexBuffer::~IndexBuffer()
	{
		// MRG_PROFILE_FUNCTION();
	}

	void IndexBuffer::bind() const
	{
		// MRG_PROFILE_FUNCTION();
	}
	void IndexBuffer::unbind() const
	{
		// MRG_PROFILE_FUNCTION();
	}
}  // namespace MRG::Vulkan
