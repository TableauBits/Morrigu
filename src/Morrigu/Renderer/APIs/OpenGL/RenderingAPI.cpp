#include "RenderingAPI.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	void RenderingAPI::init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RenderingAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { glViewport(x, y, width, height); }
	void RenderingAPI::setClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
	void RenderingAPI::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	void RenderingAPI::drawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	}
}  // namespace MRG::OpenGL