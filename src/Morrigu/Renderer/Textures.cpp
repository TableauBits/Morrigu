#include "Textures.h"

#include "Renderer/APIs/OpenGL/Textures.h"
#include "Renderer/APIs/Vulkan/Textures.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Texture2D>(width, height);
		}

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::Texture2D>(width, height);
		}

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION! ({})", RenderingAPI::getAPI()))
			return nullptr;
		}
		}
	}

	Ref<Texture2D> Texture2D::create(const std::string& path)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Texture2D>(path);
		}

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::Texture2D>(path);
		}

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION! ({})", RenderingAPI::getAPI()))
			return nullptr;
		}
		}
	}
}  // namespace MRG
