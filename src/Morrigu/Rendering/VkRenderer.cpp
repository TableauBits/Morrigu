//
// Created by mathi on 2021-04-11.
//

#include "VkRenderer.h"

namespace MRG
{
	void VkRenderer::init(const RendererSpecification& newSpec, GLFWwindow* newWindow)
	{
		spec = newSpec;
		window = newWindow;

		isInitalized = true;
	}

	void VkRenderer::cleanup()
	{
		if (isInitalized) {
			glfwDestroyWindow(window);
		}
	}
}  // namespace MRG
