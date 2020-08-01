#ifndef MRG_CLASS_CONTEXT
#define MRG_CLASS_CONTEXT

#include <GLFW/glfw3.h>

namespace MRG
{
	class Context
	{
	public:
		virtual ~Context() {}

		virtual void swapBuffers() = 0;

		[[nodiscard]] static Context* create(GLFWwindow* window);
	};
}  // namespace MRG

#endif