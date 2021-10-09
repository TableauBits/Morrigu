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
	};

	class Framebuffer
	{
	public:
		Framebuffer(vk::Device device, FramebufferData&& data);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&)      = default;

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer& operator=(Framebuffer&&) = default;

		~Framebuffer() = default;

	private:
		vk::Device m_device;
		FramebufferData m_data;
	};
}  // namespace MRG

#endif
