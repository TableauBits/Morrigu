#include "Framebuffer.h"

namespace MRG
{
	Framebuffer::Framebuffer(vk::Device deviceCopy, FramebufferData&& moveData, uint32_t initWidth, uint32_t initHeight)
	    : device{deviceCopy}, data{std::move(moveData)}, width{initWidth}, height{initHeight}
	{}

	Framebuffer::~Framebuffer()
	{
		device.destroyDescriptorPool(data.descriptorPool);
		device.destroySemaphore(data.renderSemaphore);
		device.destroyCommandPool(data.commandPool);
	}
}  // namespace MRG
