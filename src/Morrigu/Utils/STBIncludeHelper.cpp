//
// Created by Mathis Lamidey on 2021-05-30.
//

#define STB_IMAGE_IMPLEMENTATION
#include "STBIncludeHelper.h"

#include "Core/FileNames.h"
#include "Utils/Allocators.h"

namespace MRG::Utils
{
	AllocatedImage createImageFromData(vk::Device device,
	                                   vk::Queue graphicsQueue,
	                                   UploadContext uploadContext,
	                                   VmaAllocator allocator,
	                                   void* imageData,
	                                   uint32_t imageWidth,
	                                   uint32_t imageHeight,
	                                   DeletionQueue& deletionQueue)
	{
		const auto imageSize = imageWidth * imageHeight * 4;
		VkBufferCreateInfo bufferInfo{
		  .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		  .pNext                 = nullptr,
		  .flags                 = 0,
		  .size                  = imageSize,
		  .usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		  .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
		  .queueFamilyIndexCount = 0,
		  .pQueueFamilyIndices   = nullptr,
		};
		VmaAllocationCreateInfo allocationInfo{
		  .flags          = 0,
		  .usage          = VMA_MEMORY_USAGE_CPU_ONLY,
		  .requiredFlags  = 0,
		  .preferredFlags = 0,
		  .memoryTypeBits = 0,
		  .pool           = VK_NULL_HANDLE,
		  .pUserData      = nullptr,
		};
		AllocatedBuffer stagingBuffer;
		VkBuffer newRawBuffer;
		MRG_VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &allocationInfo, &newRawBuffer, &stagingBuffer.allocation, nullptr),
		             "Failed to allocate new buffer!")
		stagingBuffer.buffer = newRawBuffer;

		void* data;
		vmaMapMemory(allocator, stagingBuffer.allocation, &data);
		memcpy(data, imageData, static_cast<std::size_t>(imageSize));
		vmaUnmapMemory(allocator, stagingBuffer.allocation);

		vk::Extent3D imageExtent{
		  .width  = imageWidth,
		  .height = imageHeight,
		  .depth  = 1,
		};
		vk::Format imageFormat      = vk::Format::eR8G8B8A8Srgb;
		VkImageCreateInfo imageInfo = vk::ImageCreateInfo{
		  .imageType   = vk::ImageType::e2D,
		  .format      = imageFormat,
		  .extent      = imageExtent,
		  .mipLevels   = 1,
		  .arrayLayers = 1,
		  .samples     = vk::SampleCountFlagBits::e1,
		  .tiling      = vk::ImageTiling::eOptimal,
		  .usage       = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		};
		VmaAllocationCreateInfo imageAllocationInfo{
		  .flags          = 0,
		  .usage          = VMA_MEMORY_USAGE_GPU_ONLY,
		  .requiredFlags  = 0,
		  .preferredFlags = 0,
		  .memoryTypeBits = 0,
		  .pool           = VK_NULL_HANDLE,
		  .pUserData      = nullptr,
		};

		AllocatedImage newImage;
		VkImage newRawImage;
		vmaCreateImage(allocator, &imageInfo, &imageAllocationInfo, &newRawImage, &newImage.allocation, nullptr);
		newImage.image = newRawImage;

		Utils::Allocators::immediateSubmit(device, graphicsQueue, uploadContext, [&](vk::CommandBuffer cmdBuffer) {
			vk::ImageSubresourceRange range{
			  .aspectMask     = vk::ImageAspectFlagBits::eColor,
			  .baseMipLevel   = 0,
			  .levelCount     = 1,
			  .baseArrayLayer = 0,
			  .layerCount     = 1,
			};
			vk::ImageMemoryBarrier barrierToTransfer{
			  .srcAccessMask    = {},
			  .dstAccessMask    = vk::AccessFlagBits::eTransferWrite,
			  .oldLayout        = vk::ImageLayout::eUndefined,
			  .newLayout        = vk::ImageLayout::eTransferDstOptimal,
			  .image            = newImage.image,
			  .subresourceRange = range,
			};

			cmdBuffer.pipelineBarrier(
			  vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrierToTransfer);

			vk::BufferImageCopy copyRegion{
			  .imageSubresource =
			    {
			      .aspectMask     = vk::ImageAspectFlagBits::eColor,
			      .mipLevel       = 0,
			      .baseArrayLayer = 0,
			      .layerCount     = 1,
			    },
			  .imageExtent = imageExtent,
			};
			cmdBuffer.copyBufferToImage(stagingBuffer.buffer, newImage.image, vk::ImageLayout::eTransferDstOptimal, copyRegion);

			vk::ImageMemoryBarrier barrierToShaders{
			  .srcAccessMask    = vk::AccessFlagBits::eTransferWrite,
			  .dstAccessMask    = vk::AccessFlagBits::eShaderRead,
			  .oldLayout        = vk::ImageLayout::eTransferDstOptimal,
			  .newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal,
			  .image            = newImage.image,
			  .subresourceRange = range,
			};

			cmdBuffer.pipelineBarrier(
			  vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrierToShaders);
		});

		deletionQueue.push([=]() { vmaDestroyImage(allocator, newImage.image, newImage.allocation); });
		vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);

		return newImage;
	}

	AllocatedImage loadImageFromFile(vk::Device device,
	                                 vk::Queue graphicsQueue,
	                                 UploadContext uploadContext,
	                                 VmaAllocator allocator,
	                                 const char* file,
	                                 DeletionQueue& deletionQueue)
	{
		int texWidth, texHeight, texChannels;
		auto* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		MRG_ENGINE_ASSERT(pixels != nullptr, "Failed to load image from file: {}", file)

		const auto image = createImageFromData(device,
		                                       graphicsQueue,
		                                       uploadContext,
		                                       allocator,
		                                       pixels,
		                                       static_cast<uint32_t>(texWidth),
		                                       static_cast<uint32_t>(texHeight),
		                                       deletionQueue);

		stbi_image_free(pixels);
		return image;
	}
}  // namespace MRG::Utils
