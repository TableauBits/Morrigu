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
		uint32_t minImageCount;
		uint32_t imageCount;
		std::vector<VkImage> images;
		VkFormat imageFormat;
		VkExtent2D extent;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> frameBuffers;
	};

	struct Queue
	{
		VkQueue handle;
		uint32_t index;
	};

	struct Pipeline
	{
		VkPipeline handle;
		VkRenderPass renderPass;
		VkPipelineLayout layout;
	};

	struct Buffer
	{
		VkBuffer handle;
		VkDeviceMemory memoryHandle;
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
		Queue graphicsQueue, presentQueue;
		SwapChain swapChain;
		VkDescriptorSetLayout descriptorSetLayout;
		Pipeline pipeline;
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		std::size_t currentFrame = 0;
		std::vector<Buffer> uniformBuffers;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
	};
}  // namespace MRG::Vulkan

#endif