//
// Created by Mathis Lamidey on 2021-05-30.
//

#include "Allocators.h"

namespace MRG::Utils::Allocators
{
	AllocatedBuffer createBuffer(VmaAllocator allocator,
	                             std::size_t allocSize,
	                             vk::BufferUsageFlags bufferUsage,
	                             VmaMemoryUsage memoryUsage,
	                             DeletionQueue& deletionQueue)
	{
		VkBufferCreateInfo bufferInfo{
		  .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		  .size  = allocSize,
		  .usage = VkBufferUsageFlags(bufferUsage),
		};

		VmaAllocationCreateInfo allocationInfo{
		  .usage = memoryUsage,
		};

		AllocatedBuffer newBuffer;
		VkBuffer newRawBuffer;
		MRG_VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &allocationInfo, &newRawBuffer, &newBuffer.allocation, nullptr),
		             "Failed to allocate new buffer!")
		newBuffer.buffer = newRawBuffer;
		deletionQueue.push([=]() { vmaDestroyBuffer(allocator, newBuffer.buffer, newBuffer.allocation); });

		return newBuffer;
	}

	void immediateSubmit(vk::Device device,
	                     vk::Queue graphicsQueue,
	                     UploadContext& uploadContext,
	                     std::function<void(vk::CommandBuffer)>&& function)
	{
		vk::CommandBufferAllocateInfo cmdBufferAllocInfo{
		  .commandPool        = uploadContext.commandPool,
		  .level              = vk::CommandBufferLevel::ePrimary,
		  .commandBufferCount = 1,
		};
		const auto cmdBuffer = device.allocateCommandBuffers(cmdBufferAllocInfo).back();
		vk::CommandBufferBeginInfo beginInfo{
		  .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};

		cmdBuffer.begin(beginInfo);
		function(cmdBuffer);
		cmdBuffer.end();

		vk::SubmitInfo submitInfo{
		  .commandBufferCount = 1,
		  .pCommandBuffers    = &cmdBuffer,
		};
		graphicsQueue.submit(submitInfo, uploadContext.uploadFence);

		MRG_VK_CHECK_HPP(device.waitForFences(uploadContext.uploadFence, VK_TRUE, UINT64_MAX), "failed to wait for render fence!")
		device.resetFences(uploadContext.uploadFence);

		device.resetCommandPool(uploadContext.commandPool);
	}

}  // namespace MRG::Utils::Allocators
