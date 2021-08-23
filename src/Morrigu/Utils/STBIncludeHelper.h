//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_STBINCLUDEHELPER_H
#define MORRIGU_STBINCLUDEHELPER_H

#include "Rendering/RendererTypes.h"

#include <stb_image.h>

namespace MRG::Utils
{
	[[nodiscard]] AllocatedImage createImageFromData(vk::Device device,
	                                                 vk::Queue graphicsQueue,
	                                                 UploadContext uploadContext,
	                                                 VmaAllocator allocator,
	                                                 void* imageData,
	                                                 uint32_t imageWidth,
	                                                 uint32_t imageHeight,
	                                                 DeletionQueue& deletionQueue);

	/// They already point to the shader folder, so no path should be necessary.
	[[nodiscard]] AllocatedImage loadImageFromFile(vk::Device device,
	                                               vk::Queue graphicsQueue,
	                                               UploadContext uploadContext,
	                                               VmaAllocator allocator,
	                                               const char* file,
	                                               DeletionQueue& deletionQueue);
}  // namespace MRG::Utils

#endif  // MORRIGU_STBINCLUDEHELPER_H
