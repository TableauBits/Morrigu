//
// Created by Mathis Lamidey on 2021-09-16.
//

#include "RendererTypes.h"

#include "Utils/Commands.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace MRG
{
	AllocatedBuffer::AllocatedBuffer(VmaAllocator allocator,
	                                 std::size_t allocSize,
	                                 vk::BufferUsageFlags bufferUsage,
	                                 VmaMemoryUsage memoryUsage)
	    : allocator{allocator}
	{
		VkBufferCreateInfo bufferInfo{
		  .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		  .pNext                 = nullptr,
		  .flags                 = 0,
		  .size                  = allocSize,
		  .usage                 = VkBufferUsageFlags(bufferUsage),
		  .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
		  .queueFamilyIndexCount = 0,
		  .pQueueFamilyIndices   = nullptr,
		};

		VmaAllocationCreateInfo allocationInfo{
		  .flags          = 0,
		  .usage          = memoryUsage,
		  .requiredFlags  = 0,
		  .preferredFlags = 0,
		  .memoryTypeBits = 0,
		  .pool           = VK_NULL_HANDLE,
		  .pUserData      = nullptr,
		};

		VkBuffer newRawBuffer;
		MRG_VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &allocationInfo, &newRawBuffer, &allocation, nullptr),
		             "Failed to allocate new buffer!")
		buffer = newRawBuffer;
	}

	AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other)
	{
		allocator  = std::move(other.allocator);
		buffer     = std::move(other.buffer);
		allocation = std::move(other.allocation);

		// Just to be extra sure
		other.allocator = nullptr;
	}

	AllocatedBuffer::~AllocatedBuffer()
	{
		if (allocator != nullptr) { vmaDestroyBuffer(allocator, buffer, allocation); }
	}

	AllocatedBuffer& AllocatedBuffer::operator=(AllocatedBuffer&& other)
	{
		allocator  = std::move(other.allocator);
		buffer     = std::move(other.buffer);
		allocation = std::move(other.allocation);

		// Just to be extra sure
		other.allocator = nullptr;

		return *this;
	}

	AllocatedImage::AllocatedImage(vk::Device device,
	                               vk::Queue graphicsQueue,
	                               UploadContext uploadContext,
	                               VmaAllocator allocator,
	                               void* imageData,
	                               uint32_t imageWidth,
	                               uint32_t imageHeight)
	    : allocator{allocator}
	{
		initFromData(device, graphicsQueue, uploadContext, imageData, imageWidth, imageHeight);
	}

	AllocatedImage::AllocatedImage(
	  vk::Device device, vk::Queue graphicsQueue, UploadContext uploadContext, VmaAllocator allocator, const char* file)
	    : allocator{allocator}
	{
		int texWidth, texHeight, texChannels;
		auto* pixels = stbi_load(file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		MRG_ENGINE_ASSERT(pixels != nullptr, "Failed to load image from file: {}", file)

		initFromData(device, graphicsQueue, uploadContext, pixels, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		stbi_image_free(pixels);
	}

	void AllocatedImage::initFromData(
	  vk::Device device, vk::Queue graphicsQueue, UploadContext uploadContext, void* imageData, uint32_t imageWidth, uint32_t imageHeight)
	{
		const auto imageSize = imageWidth * imageHeight * 4;
		AllocatedBuffer stagingBuffer{allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY};

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

		VkImage newRawImage;
		vmaCreateImage(allocator, &imageInfo, &imageAllocationInfo, &newRawImage, &allocation, nullptr);
		image = newRawImage;

		Utils::Commands::immediateSubmit(device, graphicsQueue, uploadContext, [&](vk::CommandBuffer cmdBuffer) {
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
			  .image            = image,
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
			cmdBuffer.copyBufferToImage(stagingBuffer.buffer, image, vk::ImageLayout::eTransferDstOptimal, copyRegion);

			vk::ImageMemoryBarrier barrierToShaders{
			  .srcAccessMask    = vk::AccessFlagBits::eTransferWrite,
			  .dstAccessMask    = vk::AccessFlagBits::eShaderRead,
			  .oldLayout        = vk::ImageLayout::eTransferDstOptimal,
			  .newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal,
			  .image            = image,
			  .subresourceRange = range,
			};

			cmdBuffer.pipelineBarrier(
			  vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrierToShaders);
		});
	}

	AllocatedImage::AllocatedImage(AllocatedImage&& other)
	{
		allocator  = std::move(other.allocator);
		image      = std::move(other.image);
		allocation = std::move(other.allocation);

		// Just to be extra sure
		other.allocator = nullptr;
	}

	AllocatedImage::~AllocatedImage()
	{
		if (allocator != nullptr) { vmaDestroyImage(allocator, image, allocation); }
	}

	AllocatedImage& AllocatedImage::operator=(AllocatedImage&& other)
	{
		allocator  = std::move(other.allocator);
		image      = std::move(other.image);
		allocation = std::move(other.allocation);

		// Just to be extra sure
		other.allocator = nullptr;

		return *this;
	}

}  // namespace MRG
