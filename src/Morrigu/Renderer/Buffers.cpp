#include "Buffers.h"

#include "Core/Core.h"
#include "Renderer.h"
#include "Renderer/APIs/OpenGL/Buffers.h"

namespace MRG
{
	VertexBuffer* VertexBuffer::create(const float* vertices, uint32_t size)
	{
		switch (Renderer::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return new OpenGL::VertexBuffer{vertices, size};
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, "UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI());
			return nullptr;
		} break;
		}
	}

	IndexBuffer* IndexBuffer::create(const uint32_t* indices, uint32_t size)
	{
		switch (Renderer::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return new OpenGL::IndexBuffer{indices, size};
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, "UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI());
			return nullptr;
		} break;
		}
	}
}  // namespace MRG
