#include "Buffers.h"

#include "Core/Core.h"
#include "Renderer/APIs/OpenGL/Buffers.h"
#include "Renderer/Renderer.h"

namespace MRG
{
	Ref<VertexBuffer> VertexBuffer::create(const float* vertices, uint32_t size)
	{
		switch (Renderer::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::VertexBuffer>(vertices, size);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI()));
			return nullptr;
		} break;
		}
	}

	Ref<IndexBuffer> IndexBuffer::create(const uint32_t* indices, uint32_t size)
	{
		switch (Renderer::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::IndexBuffer>(indices, size);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", Renderer::getAPI()));
			return nullptr;
		} break;
		}
	}
}  // namespace MRG
