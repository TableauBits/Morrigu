//
// Created by Mathis Lamidey on 2021-09-30.
//

#ifndef MORRIGU_FRAMEBUFFER_H
#define MORRIGU_FRAMEBUFFER_H

#include "Rendering/RendererTypes.h"

namespace MRG
{
	struct FramebufferSpecification
	{
		uint32_t width;
		uint32_t height;
	};

	struct FramebufferData
	{
		// Color attachment
		AllocatedImage colorImage{};

		// Depth attachment
		AllocatedImage depthImage{};

		vk::Sampler sampler{};
		vk::Framebuffer vkHandle{};

		// Render data
		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		vk::Semaphore renderSemaphore{};

		vk::DescriptorPool descriptorPool{};
		vk::DescriptorSet level0Descriptor{};

		AllocatedBuffer timeDataBuffer{};
	};

	class Framebuffer
	{
	public:
		Framebuffer(vk::Device deviceCopy, FramebufferData&& moveData, uint32_t initWidth, uint32_t initHeight);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&)      = default;

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = default;

		~Framebuffer();

		vk::Device device;
		FramebufferData data;
		uint32_t width{}, height{};
	};
}  // namespace MRG

#endif
