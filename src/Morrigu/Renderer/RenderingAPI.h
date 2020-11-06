#ifndef MRG_CLASS_RENDERINGAPI
#define MRG_CLASS_RENDERINGAPI

#include "Renderer/VertexArray.h"

namespace MRG
{
	class RenderingAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL,
			Vulkan
		};
		[[nodiscard]] inline static auto getAPI() { return s_API; }

	private:
		static API s_API;
	};
}  // namespace MRG

#endif