//
// Created by Mathis Lamidey on 2021-04-11.
//

#ifndef MORRIGU_RENDERERTYPES_H
#define MORRIGU_RENDERERTYPES_H

#include "Core/Core.h"
#include "Utils/GLMIncludeHelper.h"
#include "Utils/VMAIncludeHelper.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <exception>

namespace MRG
{
	struct TimeData
	{
		glm::vec4 time;
	};

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
		vk::Buffer vkHandle{};
		VmaAllocation allocation{};
	};

	struct AllocatedImageSpecification
	{
		vk::Device device;
		vk::Queue graphicsQueue;
		UploadContext uploadContext;
		VmaAllocator allocator = nullptr;
		vk::ImageUsageFlags usage;
		vk::Format format = vk::Format::eR8G8B8A8Srgb;

		// From file
		const char* file = nullptr;

		// Fropm data
		void* data = nullptr;
		uint32_t width;
		uint32_t height;
	};

	class AllocatedImage
	{
	public:
		AllocatedImage() = default;
		explicit AllocatedImage(const AllocatedImageSpecification& specification);
		AllocatedImage(const AllocatedImage&) = delete;
		AllocatedImage(AllocatedImage&& other);
		~AllocatedImage();

		AllocatedImage& operator=(const AllocatedImage&) = delete;

		AllocatedImage& operator=(AllocatedImage&& other);

		AllocatedImageSpecification spec;

		VmaAllocation allocation{};
		vk::Image vkHandle{};
		vk::ImageView view{};

	private:
		void initFromData(void* imageData, uint32_t imageWidth, uint32_t imageHeight);
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
