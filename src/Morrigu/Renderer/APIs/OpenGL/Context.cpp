#include "Context.h"

#include "Core/Core.h"
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

	Context::Context(GLFWwindow* window) : m_window(window)
	{
		MRG_PROFILE_FUNCTION();

		MRG_CORE_ASSERT(window, "Window handle is null !");

		glfwMakeContextCurrent(window);
		[[maybe_unused]] const auto status = gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
		MRG_CORE_ASSERT(status, "Could not initialize glad !");

		MRG_ENGINE_INFO("Using OpenGL as a rendering API. Useful stats:");
		MRG_ENGINE_INFO("\tHardware vendor: {0}", glGetString(GL_VENDOR));
		MRG_ENGINE_INFO("\tHardware device used to render: {0}", glGetString(GL_RENDERER));
		MRG_ENGINE_INFO("\tOpenGL version: {0}", glGetString(GL_VERSION));

		MRG_CORE_ASSERT((GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5)), "OpenGL 4.5 required !")

#ifdef MRG_DEBUG
		int vMajor, vMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &vMajor);
		glGetIntegerv(GL_MINOR_VERSION, &vMinor);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void Context::swapBuffers()
	{
		MRG_PROFILE_FUNCTION();

		glfwSwapBuffers(m_window);
	}

	void Context::swapInterval(int interval)
	{
		MRG_PROFILE_FUNCTION();

		glfwSwapInterval(interval);
	}
}  // namespace MRG::OpenGL
