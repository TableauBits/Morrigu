//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_ALLOCATORS_H
#define MORRIGU_ALLOCATORS_H

#include "Rendering/VkTypes.h"

namespace MRG::Utils::Allocators
{
	[[nodiscard]] AllocatedBuffer createBuffer(VmaAllocator allocator,
	                                           std::size_t allocSize,
	                                           vk::BufferUsageFlags bufferUsage,
	                                           VmaMemoryUsage memoryUsage,
	                                           DeletionQueue& deletionQueue);

	void immediateSubmit(vk::Device device,
	                     vk::Queue graphicsQueue,
	                     UploadContext& uploadContext,
	                     std::function<void(vk::CommandBuffer)>&& function);
}  // namespace MRG::Utils::Allocators

#endif  // MORRIGU_ALLOCATORS_H
