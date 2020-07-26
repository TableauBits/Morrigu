#include "Context.h"

#include "Core/Core.h"

#include <glad/glad.h>

namespace MRG::OpenGL
{
	Context::Context(GLFWwindow* window) : m_window(window)
	{
		MRG_CORE_ASSERT(window, "Window handle is null !");

		glfwMakeContextCurrent(window);
		const auto status = gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
		MRG_CORE_ASSERT(status, "Could not initialize glad !");

		MRG_ENGINE_INFO("Using OpenGL as a rendering API. Useful stats:");
		MRG_ENGINE_INFO("\tHardware vendor: {0}", glGetString(GL_VENDOR));
		MRG_ENGINE_INFO("\tHardware device used to render: {0}", glGetString(GL_RENDERER));
		MRG_ENGINE_INFO("\tOpenGL version: {0}", glGetString(GL_VERSION));
	}

	void Context::swapBuffers() { glfwSwapBuffers(m_window); }
}  // namespace MRG