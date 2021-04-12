//
// Created by mathi on 2021-04-11.
//

#include "VkInitialize.h"

namespace MRG::VkInit
{
	vk::CommandPoolCreateInfo cmdPoolCreateInfo(vk::CommandPoolCreateFlags flags, uint32_t queueFamiliyIndex)
	{
		return vk::CommandPoolCreateInfo{.flags = flags, .queueFamilyIndex = queueFamiliyIndex};
	}

	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, vk::CommandBufferLevel level, uint32_t count)
	{
		return vk::CommandBufferAllocateInfo{.commandPool = pool, .level = level, .commandBufferCount = count};
	}
}  // namespace MRG::VkInit
