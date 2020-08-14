#include "Context.h"

#include "Core/Core.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	Context::Context(GLFWwindow* window) : m_window(window)
	{
		MRG_CORE_ASSERT(window, "Window handle is null !");

		glfwMakeContextCurrent(window);
		[[maybe_unused]] const auto status = gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
		MRG_CORE_ASSERT(status, "Could not initialize glad !");

		MRG_ENGINE_INFO("Using OpenGL as a rendering API. Useful stats:");
		MRG_ENGINE_INFO("\tHardware vendor: {0}", glGetString(GL_VENDOR));
		MRG_ENGINE_INFO("\tHardware device used to render: {0}", glGetString(GL_RENDERER));
		MRG_ENGINE_INFO("\tOpenGL version: {0}", glGetString(GL_VERSION));

#ifndef NDEBUG
		int vMajor, vMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &vMajor);
		glGetIntegerv(GL_MINOR_VERSION, &vMinor);

		MRG_CORE_ASSERT((vMajor > 4 || (vMajor == 4 && vMinor >= 5)), "OpenGL 4.5 required !")
#endif
	}

	void Context::swapBuffers() { glfwSwapBuffers(m_window); }
}  // namespace MRG::OpenGL