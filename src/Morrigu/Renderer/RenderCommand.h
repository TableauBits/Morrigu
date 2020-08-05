#ifndef MRG_CLASS_RENDERCOMMAND
#define MRG_CLASS_RENDERCOMMAND

#include "Renderer/RenderingAPI.h"

namespace MRG
{
	class RenderCommand
	{
	public:
		inline static void setClearColor(const glm::vec4& color) { s_renderingAPI->setClearColor(color); }
		inline static void clear() { s_renderingAPI->clear(); }

		inline static void drawIndexed(const Ref<VertexArray>& vertexArray) { s_renderingAPI->drawIndexed(vertexArray); }

	private:
		static RenderingAPI* s_renderingAPI;
	};
}  // namespace MRG

#endif