#include "VertexArray.h"

#include "Debug/Instrumentor.h"

namespace MRG::Vulkan
{
	VertexArray::VertexArray()
	{
		// MRG_PROFILE_FUNCTION();
	}
	VertexArray::~VertexArray()
	{
		// MRG_PROFILE_FUNCTION();
	}

	void VertexArray::bind() const
	{
		// MRG_PROFILE_FUNCTION();
	}
	void VertexArray::unbind() const
	{
		// MRG_PROFILE_FUNCTION();
	}

	void VertexArray::addVertexBuffer(const Ref<MRG::VertexBuffer>&)
	{
		// MRG_PROFILE_FUNCTION();
	}

	void VertexArray::setIndexBuffer(const Ref<MRG::IndexBuffer>&)
	{
		// MRG_PROFILE_FUNCTION();
	}
}  // namespace MRG::Vulkan