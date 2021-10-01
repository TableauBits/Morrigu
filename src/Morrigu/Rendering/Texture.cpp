//
// Created by Mathis Lamidey on 2021-05-30.
//

#include "Texture.h"

#include "Core/FileNames.h"
#include "Rendering/RendererTypes.h"

namespace MRG
{
	Texture::Texture(
	  vk::Device device, vk::Queue graphicsQueue, UploadContext uploadContext, VmaAllocator allocator, const std::string& file)
	    : m_device{device}
	{
		image = AllocatedImage{device, graphicsQueue, uploadContext, allocator, (Folders::Rendering::texturesFolder + file).c_str()};

		vk::SamplerCreateInfo samplerInfo{
		  .magFilter    = vk::Filter::eNearest,
		  .minFilter    = vk::Filter::eNearest,
		  .addressModeU = vk::SamplerAddressMode::eRepeat,
		  .addressModeV = vk::SamplerAddressMode::eRepeat,
		  .addressModeW = vk::SamplerAddressMode::eRepeat,
		};
		sampler = device.createSampler(samplerInfo);
	}

	Texture::Texture(vk::Device device,
	                 vk::Queue graphicsQueue,
	                 UploadContext uploadContext,
	                 VmaAllocator allocator,
	                 void* data,
	                 uint32_t width,
	                 uint32_t height)
	    : m_device{device}
	{
		image = AllocatedImage{device, graphicsQueue, uploadContext, allocator, data, width, height};

		vk::SamplerCreateInfo samplerInfo{
		  .magFilter    = vk::Filter::eNearest,
		  .minFilter    = vk::Filter::eNearest,
		  .addressModeU = vk::SamplerAddressMode::eClampToEdge,
		  .addressModeV = vk::SamplerAddressMode::eClampToEdge,
		  .addressModeW = vk::SamplerAddressMode::eClampToEdge,
		};
		sampler = device.createSampler(samplerInfo);
	}

	Texture::~Texture() { m_device.destroySampler(sampler); }
}  // namespace MRG
