//
// Created by mathi on 2021-04-11.
//

#include "VkInitialize.h"

namespace MRG::VkInit
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo(uint32_t queueFamiliyIndex, VkCommandPoolCreateFlags flags)
	{
        VkCommandPoolCreateInfo cmdPoolInfo{};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = queueFamiliyIndex;
        cmdPoolInfo.flags = flags;

		return cmdPoolInfo;
    }
	VkCommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level)
	{
        VkCommandBufferAllocateInfo cmdBufferInfo{};
        cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufferInfo.commandPool = pool;
        cmdBufferInfo.commandBufferCount = count;
        cmdBufferInfo.level = level;

		return cmdBufferInfo;
    }
}
