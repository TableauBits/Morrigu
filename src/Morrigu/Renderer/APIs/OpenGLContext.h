#ifndef MRG_CLASS_OPENGLCONTEXT
#define MRG_CLASS_OPENGLCONTEXT

#include "Renderer/Context.h"

#include <GLFW/glfw3.h>

namespace MRG
{
	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(GLFWwindow* window);

		void swapBuffers() override;

	private:
		GLFWwindow* m_window;
	};
}  // namespace MRG

#endif