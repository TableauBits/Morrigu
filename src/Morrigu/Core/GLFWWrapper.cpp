//
// Created by Mathis Lamidey on 2021-09-16.
//

#include "GLFWWrapper.h"

#include "Core/Core.h"

#include <GLFW/glfw3.h>

namespace MRG
{
	GLFWWrapper::GLFWWrapper()
	{
		[[maybe_unused]] const auto result = glfwInit();
		MRG_ENGINE_ASSERT(result == GLFW_TRUE, "failed to initialise GLFW!")
	}
	GLFWWrapper::~GLFWWrapper() { glfwTerminate(); }
}  // namespace MRG
