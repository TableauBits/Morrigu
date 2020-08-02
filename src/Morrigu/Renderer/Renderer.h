#ifndef MRG_CLASS_RENDERER
#define MRG_CLASS_RENDERER

#include "RenderCommand.h"

namespace MRG
{
	class Renderer
	{
	public:
		static void beginScene();
		static void endScene();

		static void submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static auto getAPI() { return RenderingAPI::getAPI(); }
	};
}  // namespace MRG

#endif