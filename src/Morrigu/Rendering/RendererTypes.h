//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_RENDERERTYPES_H
#define MORRIGU_RENDERERTYPES_H

#include "Core/Core.h"
#include "Utils/VMAIncludeHelper.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <exception>

namespace MRG
{
	struct UploadContext
	{
		vk::Fence uploadFence;
		vk::CommandPool commandPool;
	};

	class AllocatedBuffer
	{
	public:
		AllocatedBuffer() = default;
		AllocatedBuffer(VmaAllocator allocator, std::size_t allocSize, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		AllocatedBuffer(const AllocatedBuffer&) = delete;
		AllocatedBuffer(AllocatedBuffer&& other);
		~AllocatedBuffer();

		AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;

		AllocatedBuffer& operator=(AllocatedBuffer&& other);

		VmaAllocator allocator{};
		vk::Buffer buffer{};
		VmaAllocation allocation{};
	};

	class AllocatedImage
	{
	public:
		AllocatedImage() = default;
		AllocatedImage(vk::Device device,
		               vk::Queue graphicsQueue,
		               UploadContext uploadContext,
		               VmaAllocator allocator,
		               void* imageData,
		               uint32_t imageWidth,
		               uint32_t imageHeight);
		AllocatedImage(vk::Device device, vk::Queue graphicsQueue, UploadContext uploadContext, VmaAllocator allocator, const char* file);
		AllocatedImage(const AllocatedImage&) = delete;
		AllocatedImage(AllocatedImage&& other);
		~AllocatedImage();

		AllocatedImage& operator=(const AllocatedImage&) = delete;

		AllocatedImage& operator=(AllocatedImage&& other);

		VmaAllocator allocator{};
		vk::Image image{};
		VmaAllocation allocation{};

	private:
		void initFromData(vk::Device device,
		                  vk::Queue graphicsQueue,
		                  UploadContext uploadContext,
		                  void* imageData,
		                  uint32_t imageWidth,
		                  uint32_t imageHeight);
	};
}  // namespace MRG

#define MRG_VK_CHECK_HPP(expression, error_message)                                                                                        \
	{                                                                                                                                      \
		if ((expression) != vk::Result::eSuccess) { throw std::runtime_error(error_message); }                                             \
	}
// This macro is here is case we need to deal with C style VK results (for VMA initialisation, for example)
#define MRG_VK_CHECK(expression, error_message)                                                                                            \
	{                                                                                                                                      \
		if ((expression) != VK_SUCCESS) { throw std::runtime_error(error_message); }                                                       \
	}

#endif  // MORRIGU_RENDERERTYPES_H
