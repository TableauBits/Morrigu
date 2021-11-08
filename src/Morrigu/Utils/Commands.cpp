//
// Created by Mathis Lamidey on 2021-05-30.
//

#include "Commands.h"

namespace MRG::Utils::Commands
{
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

		MRG_VK_CHECK_HPP(device.waitForFences(uploadContext.uploadFence, VK_TRUE, UINT64_MAX), "failed to wait for command fence!")
		device.resetFences(uploadContext.uploadFence);

		device.resetCommandPool(uploadContext.commandPool);
	}

}  // namespace MRG::Utils::Commands
