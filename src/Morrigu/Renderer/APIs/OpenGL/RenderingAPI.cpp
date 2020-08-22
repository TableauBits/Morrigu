#include "RenderingAPI.h"

#include "Debug/Instrumentor.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	void RenderingAPI::init()
	{
		MRG_PROFILE_FUNCTION();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void RenderingAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { glViewport(x, y, width, height); }
	void RenderingAPI::setClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
	void RenderingAPI::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	void RenderingAPI::drawIndexed(const Ref<VertexArray>& vertexArray)
	{
		MRG_PROFILE_FUNCTION();

		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}  // namespace MRG::OpenGL