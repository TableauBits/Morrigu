#ifndef MRG_CLASS_RENDERCOMMAND
#define MRG_CLASS_RENDERCOMMAND

#include "Renderer/RenderingAPI.h"

namespace MRG
{
	class RenderCommand
	{
	public:
		inline static void init() { s_renderingAPI->init(); }
		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_renderingAPI->setViewport(x, y, width, height);
		}

		inline static void setClearColor(const glm::vec4& color) { s_renderingAPI->setClearColor(color); }
		inline static void clear() { s_renderingAPI->clear(); }

		inline static void drawIndexed(const Ref<VertexArray>& vertexArray) { s_renderingAPI->drawIndexed(vertexArray); }

	private:
		static RenderingAPI* s_renderingAPI;
	};
}  // namespace MRG

#endif