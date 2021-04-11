//
// Created by mathi on 2021-04-11.
//

#ifndef MORRIGU_VKINITIALIZE_H
#define MORRIGU_VKINITIALIZE_H

#include "Rendering/VkTypes.h"

namespace MRG::VkInit
{
	VkCommandPoolCreateInfo cmdPoolCreateInfo(uint32_t queueFamiliyIndex, VkCommandPoolCreateFlags flags);
	VkCommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level);
}  // namespace MRG::VkInit

#endif  // MORRIGU_VKINITIALIZE_H
