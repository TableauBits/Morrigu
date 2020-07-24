#include "OpenGLContext.h"

#include "Core/Core.h"

#include <glad/glad.h>

namespace MRG
{
	OpenGLContext::OpenGLContext(GLFWwindow* window) : m_window(window)
	{
		MRG_CORE_ASSERT(window, "Window handle is null !");

		glfwMakeContextCurrent(window);
		const auto status = gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
		MRG_CORE_ASSERT(status, "Could not initialize glad !");
	}

	void OpenGLContext::swapBuffers() { glfwSwapBuffers(m_window); }
}  // namespace MRG