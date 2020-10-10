#ifndef MRG_VULKAN_IMPL_WINDOWPROPERTIES
#define MRG_VULKAN_IMPL_WINDOWPROPERTIES

#include "Renderer/WindowProperties.h"

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace MRG::Vulkan
{
	struct SwapChain
	{
		VkSwapchainKHR handle;
		std::vector<VkImage> images;
		VkFormat imageFormat;
		VkExtent2D extent;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> frameBuffers;
	};

	struct Pipeline
	{
		VkPipeline handle;
		VkRenderPass renderPass;
		VkPipelineLayout layout;
	};

	class WindowProperties : public MRG::WindowProperties
	{
	public:
		WindowProperties(const std::string& newTitle, unsigned int newWidth, unsigned int newHeight, bool newVSync)
		    : MRG::WindowProperties{newTitle, newWidth, newHeight, newVSync}
		{}

		VkInstance instance;
		VkDebugUtilsMessengerEXT messenger;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue, presentQueue;
		SwapChain swapChain;
		Pipeline pipeline;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}  // namespace MRG::Vulkan

#endif