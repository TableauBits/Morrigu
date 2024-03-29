//
// Created by Mathis Lamidey on 2021-05-30.
//

#ifndef MORRIGU_TEXTURE_H
#define MORRIGU_TEXTURE_H

#include "Rendering/RendererTypes.h"

#include <imgui.h>

namespace MRG
{
	class Texture
	{
	public:
		Texture(vk::Device device, vk::Queue graphicsQueue, UploadContext uploadContext, VmaAllocator allocator, const std::string& file);
		Texture(vk::Device device,
		        vk::Queue graphicsQueue,
		        UploadContext uploadContext,
		        VmaAllocator allocator,
		        void* data,
		        uint32_t width,
		        uint32_t height);

		~Texture();

		[[nodiscard]] ImTextureID getImTexID();

		std::string path{};

		AllocatedImage image;
		vk::Sampler sampler;

	private:
		vk::Device m_device;
		ImTextureID m_imTexID{nullptr};
	};
}  // namespace MRG

#endif  // MORRIGU_TEXTURE_H
