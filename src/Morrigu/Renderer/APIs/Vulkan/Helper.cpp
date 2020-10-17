#include "Helper.h"

#include "Core/Core.h"
#include "Core/Logger.h"

namespace MRG::Vulkan
{
	[[nodiscard]] SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice device, const VkSurfaceKHR surface)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t count{};

		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
		if (count != 0) {
			details.formats.resize(count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, details.formats.data());
		}

		count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
		if (count != 0) {
			details.presentModes.resize(count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, details.presentModes.data());
		}

		return details;
	}

	[[nodiscard]] uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
			if ((typeFilter & MRG_BIT(i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}

	void createBuffer(VkDevice device,
	                  VkPhysicalDevice physicalDevice,
	                  VkDeviceSize size,
	                  VkBufferUsageFlags usage,
	                  VkMemoryPropertyFlags properties,
	                  Buffer& buffer)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		MRG_VKVALIDATE(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.handle), "failed to create vertex buffer!");
		MRG_ENGINE_TRACE("Vertex buffer succesfully created");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer.handle, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		MRG_VKVALIDATE(vkAllocateMemory(device, &allocInfo, nullptr, &buffer.memoryHandle), "failed to allocate vertex buffer memory");
		MRG_ENGINE_TRACE("Vertex buffer memory successfully allocated");

		vkBindBufferMemory(device, buffer.handle, buffer.memoryHandle, 0);
	}

	void copyBuffer(const MRG::Vulkan::WindowProperties* data, MRG::Vulkan::Buffer src, MRG::Vulkan::Buffer dst, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = data->commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(data->device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, src.handle, dst.handle, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(data->graphicsQueue.handle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(data->graphicsQueue.handle);

		vkFreeCommandBuffers(data->device, data->commandPool, 1, &commandBuffer);
	}
}  // namespace MRG::Vulkan