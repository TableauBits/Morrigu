#ifndef MRG_VKHELPER
#define MRG_VKHELPER

#include "Renderer/APIs/Vulkan/WindowProperties.h"

#include <vulkan/vulkan.hpp>

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

	struct Buffer
	{
		VkBuffer handle;
		VkDeviceMemory memoryHandle;
	};

	[[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface);

	[[nodiscard]] uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDevice device,
	                  VkPhysicalDevice physicalDevice,
	                  VkDeviceSize size,
	                  VkBufferUsageFlags usage,
	                  VkMemoryPropertyFlags properties,
	                  Buffer& buffer);

	void copyBuffer(const MRG::Vulkan::WindowProperties* data, MRG::Vulkan::Buffer src, MRG::Vulkan::Buffer dst, VkDeviceSize size);
}  // namespace MRG::Vulkan

#endif