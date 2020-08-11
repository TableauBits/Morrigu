#ifndef MRG_OPENGL_IMPL_RENDRINGAPI
#define MRG_OPENGL_IMPL_RENDRINGAPI

#include "Renderer/RenderingAPI.h"

namespace MRG::OpenGL
{
	class RenderingAPI : public MRG::RenderingAPI
	{
	public:
		void init() override;

		void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void setClearColor(const glm::vec4& color) override;
		void clear() override;

		void drawIndexed(const Ref<VertexArray>& vertexArray) override;
	};
}  // namespace MRG::OpenGL

#endif