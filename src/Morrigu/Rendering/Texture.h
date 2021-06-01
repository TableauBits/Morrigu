//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_TEXTURE_H
#define MORRIGU_TEXTURE_H

#include "Rendering/VkTypes.h"

namespace MRG
{
	class Texture
	{
	public:
		Texture(vk::Device device,
		        vk::Queue graphicsQueue,
		        UploadContext uploadContext,
		        VmaAllocator allocator,
		        const std::string& file,
		        DeletionQueue& deletionQueue);

		AllocatedImage image;
		vk::ImageView imageView;
		vk::Sampler sampler;
	};
}  // namespace MRG

#endif  // MORRIGU_TEXTURE_H
