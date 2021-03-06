#ifndef MRG_HELPER_VULKANHPP
#define MRG_HELPER_VULKANHPP

#include "Core/Warnings.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_SHADOW
#include <vulkan/vulkan.hpp>
DISABLE_WARNING_POP

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
		std::vector<std::array<VkFramebuffer, 3>> frameBuffers;
		LightVulkanImage depthBuffer{};
	};

	struct Queue
	{
		VkQueue handle;
		uint32_t index;
	};

	struct Buffer
	{
		VkBuffer handle;
		VkDeviceMemory memoryHandle;
	};

}  // namespace MRG::Vulkan

#endif
