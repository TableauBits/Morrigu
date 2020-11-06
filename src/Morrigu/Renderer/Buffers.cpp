#include "Buffers.h"

#include "Core/Core.h"
#include "Renderer/APIs/OpenGL/Buffers.h"
#include "Renderer/APIs/Vulkan/Buffers.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Ref<VertexBuffer> VertexBuffer::create(const void* vertices, std::size_t size)
	{
		return VertexBuffer::create(vertices, static_cast<uint32_t>(size));
	}

	Ref<VertexBuffer> VertexBuffer::create(const void* vertices, uint32_t size)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::VertexBuffer>(vertices, size);
		} break;

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::VertexBuffer>(vertices, size);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
			return nullptr;
		} break;
		}
	}

	Ref<IndexBuffer> IndexBuffer::create(const uint32_t* indices, std::size_t size)
	{
		return IndexBuffer::create(indices, static_cast<uint32_t>(size));
	}

	Ref<IndexBuffer> IndexBuffer::create(const uint32_t* indices, uint32_t size)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::IndexBuffer>(indices, size);
		} break;

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::IndexBuffer>(indices, size);
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
			return nullptr;
		} break;
		}
	}
}  // namespace MRG
