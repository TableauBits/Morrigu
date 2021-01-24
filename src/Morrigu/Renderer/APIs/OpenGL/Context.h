#ifndef MRG_OPENGL_IMPL_CONTEXT
#define MRG_OPENGL_IMPL_CONTEXT

#include "Renderer/Context.h"

#include <GLFW/glfw3.h>

namespace MRG::OpenGL
{
	class Context : public MRG::Context
	{
	public:
		explicit Context(GLFWwindow* window);
		Context(const Context&) = delete;
		Context(Context&&) = delete;
		~Context() override = default;

		Context& operator=(const Context&) = delete;
		Context& operator=(Context&&) = delete;

		void swapBuffers() override;
		void swapInterval(int interval) override;

	private:
		GLFWwindow* m_window;
	};
}  // namespace MRG::OpenGL

#endif
