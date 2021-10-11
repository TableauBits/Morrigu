#include "Framebuffer.h"

#include <Vendor/ImGui/bindings/imgui_impl_vulkan.h>

namespace MRG
{
	Framebuffer::Framebuffer(
	  vk::Device deviceCopy, FramebufferData&& moveData, uint32_t initWidth, uint32_t initHeight, const std::array<float, 4>& clear)
	    : device{deviceCopy}, data{std::move(moveData)}, width{initWidth}, height{initHeight}, clearColor{clear}
	{}

	Framebuffer::~Framebuffer()
	{
		device.destroySampler(data.sampler);

		device.destroyDescriptorPool(data.descriptorPool);
		device.destroyFence(data.renderFence);
		device.destroyCommandPool(data.commandPool);
	}

	ImTextureID Framebuffer::getImTexID()
	{
		if (imTexID == nullptr) {
			imTexID = ImGui_ImplVulkan_AddTexture(data.sampler, data.colorImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		return imTexID;
	}
}  // namespace MRG
