//
// Created by Mathis Lamidey on 2021-05-30.
//

#include "Texture.h"

#include "Core/FileNames.h"
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

		vk::ImageViewCreateInfo imageViewInfo{
		  .image    = image.image,
		  .viewType = vk::ImageViewType::e2D,
		  .format   = vk::Format::eR8G8B8A8Srgb,
		  .subresourceRange =
		    {
		      .aspectMask     = vk::ImageAspectFlagBits::eColor,
		      .baseMipLevel   = 0,
		      .levelCount     = 1,
		      .baseArrayLayer = 0,
		      .layerCount     = 1,
		    },
		};
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

	Texture::Texture(vk::Device device,
	                 vk::Queue graphicsQueue,
	                 UploadContext uploadContext,
	                 VmaAllocator allocator,
	                 void* data,
	                 uint32_t width,
	                 uint32_t height,
	                 DeletionQueue& deletionQueue)
	{
		image = Utils::createImageFromData(device, graphicsQueue, uploadContext, allocator, data, width, height, deletionQueue);

		vk::ImageViewCreateInfo imageViewInfo{
		  .image    = image.image,
		  .viewType = vk::ImageViewType::e2D,
		  .format   = vk::Format::eR8G8B8A8Srgb,
		  .subresourceRange =
		    {
		      .aspectMask     = vk::ImageAspectFlagBits::eColor,
		      .baseMipLevel   = 0,
		      .levelCount     = 1,
		      .baseArrayLayer = 0,
		      .layerCount     = 1,
		    },
		};
		imageView                  = device.createImageView(imageViewInfo);
		const auto imageViewBackup = imageView;
		deletionQueue.push([=]() { device.destroyImageView(imageViewBackup); });

		vk::SamplerCreateInfo samplerInfo{
		  .magFilter    = vk::Filter::eNearest,
		  .minFilter    = vk::Filter::eNearest,
		  .addressModeU = vk::SamplerAddressMode::eClampToEdge,
		  .addressModeV = vk::SamplerAddressMode::eClampToEdge,
		  .addressModeW = vk::SamplerAddressMode::eClampToEdge,
		};
		sampler                  = device.createSampler(samplerInfo);
		const auto samplerBackup = sampler;
		deletionQueue.push([=]() { device.destroySampler(samplerBackup); });
	}
}  // namespace MRG
