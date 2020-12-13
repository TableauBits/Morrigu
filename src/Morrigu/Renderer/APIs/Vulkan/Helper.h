#ifndef MRG_VKHELPER
#define MRG_VKHELPER

#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"

#include <optional>
#include <stdexcept>

#define MRG_VKVALIDATE(expression, failure_message)                                                                                        \
	if (expression != VK_SUCCESS) {                                                                                                        \
		throw std::runtime_error(failure_message);                                                                                         \
	}

namespace MRG::Vulkan
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		[[nodiscard]] bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	[[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface);

	[[nodiscard]] uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDevice device,
	                  VkPhysicalDevice physicalDevice,
	                  VkDeviceSize size,
	                  VkBufferUsageFlags usage,
	                  VkMemoryPropertyFlags properties,
	                  Buffer& buffer);

	[[nodiscard]] VkCommandBuffer beginSingleTimeCommand(const MRG::Vulkan::WindowProperties* data);
	void endSingleTimeCommand(const MRG::Vulkan::WindowProperties* data, VkCommandBuffer commandBuffer);
	void copyBuffer(const MRG::Vulkan::WindowProperties* data, MRG::Vulkan::Buffer src, MRG::Vulkan::Buffer dst, VkDeviceSize size);

	void createImage(VkPhysicalDevice physicalDevice,
	                 VkDevice device,
	                 uint32_t width,
	                 uint32_t height,
	                 VkFormat format,
	                 VkImageTiling tiling,
	                 VkImageUsageFlags usage,
	                 VkMemoryPropertyFlags properties,
	                 VkImage& image,
	                 VkDeviceMemory& imageMemory);
	void transitionImageLayout(const MRG::Vulkan::WindowProperties* data, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	void transitionImageLayoutInline(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(const MRG::Vulkan::WindowProperties* data, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	[[nodiscard]] VkImageView createImageView(const VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
}  // namespace MRG::Vulkan

#endif