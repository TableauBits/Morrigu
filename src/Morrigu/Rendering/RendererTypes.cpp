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
		vkHandle = newRawBuffer;
	}

	AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other)
	{
		allocator  = std::move(other.allocator);
		vkHandle   = std::move(other.vkHandle);
		allocation = std::move(other.allocation);

		// necessary to indicate we've taken ownership
		other.allocator = nullptr;
	}

	AllocatedBuffer::~AllocatedBuffer()
	{
		if (allocator != nullptr) { vmaDestroyBuffer(allocator, vkHandle, allocation); }
	}

	AllocatedBuffer& AllocatedBuffer::operator=(AllocatedBuffer&& other)
	{
		allocator  = std::move(other.allocator);
		vkHandle   = std::move(other.vkHandle);
		allocation = std::move(other.allocation);

		// necessary to indicate we've taken ownership
		other.allocator = nullptr;

		return *this;
	}

	AllocatedImage::AllocatedImage(const AllocatedImageSpecification& specification) : spec{specification}
	{
		if (spec.file != nullptr) {
			int texWidth, texHeight, texChannels;
			auto* pixels = stbi_load(spec.file, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			MRG_ENGINE_ASSERT(pixels != nullptr, "Failed to load image from file: {}", spec.file)

			initFromData(pixels, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

			stbi_image_free(pixels);
		} else {
			initFromData(nullptr, spec.width, spec.height);
		}
	}

	void AllocatedImage::initFromData(void* imageData, uint32_t imageWidth, uint32_t imageHeight)
	{
		vk::Extent3D imageExtent{
		  .width  = imageWidth,
		  .height = imageHeight,
		  .depth  = 1,
		};
		VkImageCreateInfo imageInfo = vk::ImageCreateInfo{
		  .imageType   = vk::ImageType::e2D,
		  .format      = spec.format,
		  .extent      = imageExtent,
		  .mipLevels   = 1,
		  .arrayLayers = 1,
		  .samples     = vk::SampleCountFlagBits::e1,
		  .tiling      = vk::ImageTiling::eOptimal,
		  .usage       = spec.usage,  // vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
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
		vmaCreateImage(spec.allocator, &imageInfo, &imageAllocationInfo, &newRawImage, &allocation, nullptr);
		vkHandle = newRawImage;

		if (imageData != nullptr) {
			const auto imageSize = imageWidth * imageHeight * 4;
			AllocatedBuffer stagingBuffer{spec.allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY};

			void* data;
			vmaMapMemory(spec.allocator, stagingBuffer.allocation, &data);
			memcpy(data, imageData, static_cast<std::size_t>(imageSize));
			vmaUnmapMemory(spec.allocator, stagingBuffer.allocation);

			Utils::Commands::immediateSubmit(spec.device, spec.graphicsQueue, spec.uploadContext, [&](vk::CommandBuffer cmdBuffer) {
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
				  .image            = vkHandle,
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
				cmdBuffer.copyBufferToImage(stagingBuffer.vkHandle, vkHandle, vk::ImageLayout::eTransferDstOptimal, copyRegion);

				vk::ImageMemoryBarrier barrierToShaders{
				  .srcAccessMask    = vk::AccessFlagBits::eTransferWrite,
				  .dstAccessMask    = vk::AccessFlagBits::eShaderRead,
				  .oldLayout        = vk::ImageLayout::eTransferDstOptimal,
				  .newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal,
				  .image            = vkHandle,
				  .subresourceRange = range,
				};

				cmdBuffer.pipelineBarrier(
				  vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrierToShaders);
			});
		} else {
			Utils::Commands::immediateSubmit(spec.device, spec.graphicsQueue, spec.uploadContext, [&](vk::CommandBuffer cmdBuffer) {
				vk::ImageSubresourceRange range{
				  .aspectMask     = vk::ImageAspectFlagBits::eColor,
				  .baseMipLevel   = 0,
				  .levelCount     = 1,
				  .baseArrayLayer = 0,
				  .layerCount     = 1,
				};

				vk::ImageMemoryBarrier barrierToShaders{
				  .srcAccessMask    = vk::AccessFlagBits::eTransferWrite,
				  .dstAccessMask    = vk::AccessFlagBits::eShaderRead,
				  .oldLayout        = vk::ImageLayout::eUndefined,
				  .newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal,
				  .image            = vkHandle,
				  .subresourceRange = range,
				};

				cmdBuffer.pipelineBarrier(
				  vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrierToShaders);
			});
		}

		vk::ImageViewCreateInfo imageViewInfo{
		  .image    = vkHandle,
		  .viewType = vk::ImageViewType::e2D,
		  .format   = spec.format,
		  .subresourceRange =
		    {
		      .aspectMask     = vk::ImageAspectFlagBits::eColor,
		      .baseMipLevel   = 0,
		      .levelCount     = 1,
		      .baseArrayLayer = 0,
		      .layerCount     = 1,
		    },
		};
		view = spec.device.createImageView(imageViewInfo);
	}

	AllocatedImage::AllocatedImage(AllocatedImage&& other)
	{
		spec       = std::move(other.spec);
		allocation = std::move(other.allocation);
		vkHandle   = std::move(other.vkHandle);
		view       = std::move(other.view);

		// necessary to indicate we've taken ownership
		other.spec.allocator = nullptr;
	}

	AllocatedImage::~AllocatedImage()
	{
		if (spec.allocator != nullptr) {
			spec.device.destroyImageView(view);
			vmaDestroyImage(spec.allocator, vkHandle, allocation);
		}
	}

	AllocatedImage& AllocatedImage::operator=(AllocatedImage&& other)
	{
		spec       = std::move(other.spec);
		allocation = std::move(other.allocation);
		vkHandle   = std::move(other.vkHandle);
		view       = std::move(other.view);

		// necessary to indicate we've taken ownership
		other.spec.allocator = nullptr;

		return *this;
	}

}  // namespace MRG
