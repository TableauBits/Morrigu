#include "Context.h"

#include "Core/Core.h"
#include "Debug/Instrumentor.h"

namespace MRG::Vulkan
{
	Context::Context([[maybe_unused]] GLFWwindow* window)
	{
		MRG_PROFILE_FUNCTION();

		MRG_CORE_ASSERT(window, "Window handle is null !");

		MRG_ENGINE_INFO("Using Vulkan as a rendering API. Other useful stats coming (hopefully) soon!");
	}

	void Context::swapBuffers()
	{
		// MRG_PROFILE_FUNCTION();
	}

	void Context::swapInterval(int) {}
}  // namespace MRG::Vulkan