#ifndef MRG_OPENGL_IMPL_CONTEXT
#define MRG_OPENGL_IMPL_CONTEXT

#include "Renderer/Context.h"

#include <GLFW/glfw3.h>

namespace MRG::OpenGL
{
	class Context : public MRG::Context
	{
	public:
		Context(GLFWwindow* window);

		void swapBuffers() override;

	private:
		GLFWwindow* m_window;
	};
}  // namespace MRG::OpenGL

#endif