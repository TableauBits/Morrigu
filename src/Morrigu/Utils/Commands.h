//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_ALLOCATORS_H
#define MORRIGU_ALLOCATORS_H

#include "Rendering/RendererTypes.h"

namespace MRG::Utils::Commands
{
	void immediateSubmit(vk::Device device,
	                     vk::Queue graphicsQueue,
	                     UploadContext& uploadContext,
	                     std::function<void(vk::CommandBuffer)>&& function);
}  // namespace MRG::Utils::Commands

#endif  // MORRIGU_ALLOCATORS_H
