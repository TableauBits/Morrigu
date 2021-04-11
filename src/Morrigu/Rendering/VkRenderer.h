//
// Created by mathi on 2021-04-11.
//

#ifndef MORRIGU_VKRENDERER_H
#define MORRIGU_VKRENDERER_H

#include "Rendering/VkTypes.h"
#include "Events/ApplicationEvent.h"

#include <GLFW/glfw3.h>

#include <string>

namespace MRG
{
	struct RendererSpecification
	{
		std::string applicationName;
		int windowWidth;
		int windowHeight;
		VkPresentModeKHR presentMode;
	};

	class VkRenderer
	{
	public:
		void init(const RendererSpecification&, GLFWwindow*);

		void beginFrame(){};
		void endFrame(){};

		void cleanup();

		void onResize();

		RendererSpecification spec;

		bool isInitalized{false};
		int frameNumber{0};
		GLFWwindow* window{nullptr};

	private:
		VkInstance m_instance{};
		VkDebugUtilsMessengerEXT m_debugMessenger{};
		VkPhysicalDevice m_GPU{};
		VkDevice m_device{};
		VkSurfaceKHR m_surface{};

		VkSwapchainKHR m_swapchain{};
		VkFormat m_swapchainFormat{};
		std::vector<VkImage> m_swapchainImages{};
		std::vector<VkImageView> m_swapchainImageViews{};

		void initVulkan();
		void initSwapchain();

		void destroySwapchain();
	};
}  // namespace MRG

#endif  // MORRIGU_VKRENDERER_H
