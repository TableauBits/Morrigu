//
// Created by mathi on 2021-04-11.
//

#ifndef MORRIGU_VKINITIALIZE_H
#define MORRIGU_VKINITIALIZE_H

#include "Rendering/VkTypes.h"

namespace MRG::VkInit
{
	vk::CommandPoolCreateInfo cmdPoolCreateInfo(vk::CommandPoolCreateFlags flags, uint32_t queueFamiliyIndex);
	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, vk::CommandBufferLevel level, uint32_t count);
}  // namespace MRG::VkInit

#endif  // MORRIGU_VKINITIALIZE_H
