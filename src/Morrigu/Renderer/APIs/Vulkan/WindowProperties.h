#ifndef MRG_VULKAN_IMPL_WINDOWPROPERTIES
#define MRG_VULKAN_IMPL_WINDOWPROPERTIES

#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"
#include "Renderer/WindowProperties.h"

#include <string>
#include <vector>

namespace MRG::Vulkan
{
	struct LightVulkanImage
	{
		VkImage handle;
		VkDeviceMemory memoryHandle;
		VkImageView imageView;
	};

	struct SwapChain
	{
		VkSwapchainKHR handle{};
		uint32_t minImageCount{};
		uint32_t imageCount{};
		std::vector<VkImage> images;
		VkFormat imageFormat{};
		VkExtent2D extent{};
		std::vector<VkImageView> imageViews;
		std::vector<LightVulkanImage> objectIDBuffers;
		std::vector<std::array<VkFramebuffer, 3>> frameBuffers;
		LightVulkanImage depthBuffer{};
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
		WindowProperties(const char* newTitle, uint32_t newWidth, uint32_t newHeight, bool newVSync)
		    : MRG::WindowProperties{newTitle, newWidth, newHeight, newVSync}
		{}

		VkInstance instance{};
		VkDebugUtilsMessengerEXT messenger{};
		VkSurfaceKHR surface{};
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device{};
		Queue graphicsQueue{}, presentQueue{};
		SwapChain swapChain;
		VkDescriptorSetLayout descriptorSetLayout{};
		Pipeline clearingPipeline{};
		Pipeline renderingPipeline{};
		VkRenderPass ImGuiRenderPass{};
		VkCommandPool commandPool{};
		std::vector<std::array<VkCommandBuffer, 3>> commandBuffers;
		std::size_t currentFrame = 0;
		VkPushConstantRange pushConstantRanges{};
		VkDescriptorPool ImGuiPool{};
	};
}  // namespace MRG::Vulkan

#endif
