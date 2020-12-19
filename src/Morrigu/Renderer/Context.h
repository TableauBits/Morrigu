#ifndef MRG_CLASS_CONTEXT
#define MRG_CLASS_CONTEXT

#include "Core/Core.h"
#include <GLFW/glfw3.h>

namespace MRG
{
	class Context
	{
	public:
		virtual ~Context() {}

		virtual void swapBuffers() = 0;
		virtual void swapInterval(int interval) = 0;

		[[nodiscard]] static Scope<Context> create(GLFWwindow* window);
	};
}  // namespace MRG

#endif
