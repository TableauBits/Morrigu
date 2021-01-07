#include "Helper.h"

namespace MRG::Vulkan
{
	[[nodiscard]] SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
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

		MRG_VKVALIDATE(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.handle), "failed to create vertex buffer!")

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer.handle, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		MRG_VKVALIDATE(vkAllocateMemory(device, &allocInfo, nullptr, &buffer.memoryHandle), "failed to allocate vertex buffer memory")

		vkBindBufferMemory(device, buffer.handle, buffer.memoryHandle, 0);
	}

	[[nodiscard]] VkCommandBuffer beginSingleTimeCommand(const MRG::Vulkan::WindowProperties* data)
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

		return commandBuffer;
	}

	void endSingleTimeCommand(const MRG::Vulkan::WindowProperties* data, VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(data->graphicsQueue.handle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(data->graphicsQueue.handle);

		vkFreeCommandBuffers(data->device, data->commandPool, 1, &commandBuffer);
	}

	void copyBuffer(const MRG::Vulkan::WindowProperties* data, MRG::Vulkan::Buffer src, MRG::Vulkan::Buffer dst, VkDeviceSize size)
	{
		const auto commandBuffer = beginSingleTimeCommand(data);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, src.handle, dst.handle, 1, &copyRegion);

		endSingleTimeCommand(data, commandBuffer);
	}

	void createImage(VkPhysicalDevice physicalDevice,
	                 VkDevice device,
	                 uint32_t width,
	                 uint32_t height,
	                 VkFormat format,
	                 VkImageTiling tiling,
	                 VkImageUsageFlags usage,
	                 VkMemoryPropertyFlags properties,
	                 VkImage& image,
	                 VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		MRG_VKVALIDATE(vkCreateImage(device, &imageInfo, nullptr, &image), "failed to create image!")

		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		MRG_VKVALIDATE(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory), "failed to allocate image memory!")

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	void transitionImageLayout(const MRG::Vulkan::WindowProperties* data, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		const auto commandBuffer = beginSingleTimeCommand(data);

		transitionImageLayoutInline(commandBuffer, image, oldLayout, newLayout);

		endSingleTimeCommand(data, commandBuffer);
	}

	void transitionImageLayoutInline(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkPipelineStageFlags sourceStage{}, destinationStage{};
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		switch (oldLayout) {
		case VK_IMAGE_LAYOUT_UNDEFINED: {
			barrier.srcAccessMask = 0;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		} break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
			barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // Hard coded for now, TODO: expose this somehow
		} break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		} break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} break;

		default: {
			MRG_CORE_ASSERT(false, "Layout transition not currently supported!")
		} break;
		}

		switch (newLayout) {
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;  // Hard coded for now, TODO: expose this somehow
		} break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		} break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: {
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} break;

		default: {
			MRG_CORE_ASSERT(false, "Layout transition not currently supported!")
		} break;
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

	void copyBufferToImage(const MRG::Vulkan::WindowProperties* data, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		const auto commandBuffer = beginSingleTimeCommand(data);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = {width, height, 1};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommand(data, commandBuffer);
	}

	[[nodiscard]] VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		MRG_VKVALIDATE(vkCreateImageView(device, &viewInfo, nullptr, &imageView), "failed to create texture image view!")

		return imageView;
	}
}  // namespace MRG::Vulkan
