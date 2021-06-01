//
// Created by Mathis Lamidey on 2021-05-30.
//

#include "Texture.h"

#include "Core/FileNames.h"
#include "Rendering/VkInitialize.h"
#include "Utils/STBIncludeHelper.h"

namespace MRG
{
	Texture::Texture(vk::Device device,
	                 vk::Queue graphicsQueue,
	                 UploadContext uploadContext,
	                 VmaAllocator allocator,
	                 const std::string& file,
	                 DeletionQueue& deletionQueue)
	{
		image = Utils::loadImageFromFile(
		  device, graphicsQueue, uploadContext, allocator, (Folders::Rendering::texturesFolder + file).c_str(), deletionQueue);

		const auto imageViewInfo   = VkInit::imageViewCreateInfo(vk::Format::eR8G8B8A8Srgb, image.image, vk::ImageAspectFlagBits::eColor);
		imageView                  = device.createImageView(imageViewInfo);
		const auto imageViewBackup = imageView;
		deletionQueue.push([=]() { device.destroyImageView(imageViewBackup); });

		vk::SamplerCreateInfo samplerInfo{
		  .magFilter    = vk::Filter::eNearest,
		  .minFilter    = vk::Filter::eNearest,
		  .addressModeU = vk::SamplerAddressMode::eRepeat,
		  .addressModeV = vk::SamplerAddressMode::eRepeat,
		  .addressModeW = vk::SamplerAddressMode::eRepeat,
		};
		sampler                  = device.createSampler(samplerInfo);
		const auto samplerBackup = sampler;
		deletionQueue.push([=]() { device.destroySampler(samplerBackup); });
	}
}  // namespace MRG
