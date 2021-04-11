//
// Created by mathi on 2021-04-11.
//

#ifndef MORRIGU_VKRENDERER_H
#define MORRIGU_VKRENDERER_H

#include <GLFW/glfw3.h>

#include <string>

namespace MRG
{
	struct RendererSpecification
	{
        std::string windowName;
		int windowWidth;
		int windowHeight;
	};

	class VkRenderer
	{
	public:
		void init(const RendererSpecification&, GLFWwindow*);

		void beginFrame(){};
		void endFrame(){};

		void cleanup();

		RendererSpecification spec;

		bool isInitalized{false};
		int frameNumber{0};
		GLFWwindow* window{nullptr};
	};
}  // namespace MRG

#endif  // MORRIGU_VKRENDERER_H
