#include "RenderingAPI.h"

#include "Debug/Instrumentor.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	void openGLMessageCallback(GLenum, GLenum, GLuint, GLenum severity, GLsizei, [[maybe_unused]] const GLchar* message, const void*)
	{
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: {
			MRG_ENGINE_TRACE(message);
		} break;
		case GL_DEBUG_SEVERITY_LOW: {
			MRG_ENGINE_WARN(message);
		} break;
		case GL_DEBUG_SEVERITY_MEDIUM: {
			MRG_ENGINE_ERROR(message);
		} break;
		case GL_DEBUG_SEVERITY_HIGH: {
			MRG_ENGINE_FATAL(message);
		} break;
		default: {
			MRG_ENGINE_INFO(message);
		} break;
		}
	}

	void RenderingAPI::init()
	{
		MRG_PROFILE_FUNCTION();

#ifdef MRG_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

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