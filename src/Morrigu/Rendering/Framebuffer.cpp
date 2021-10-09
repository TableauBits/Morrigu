#include "Framebuffer.h"

namespace MRG
{
	Framebuffer::Framebuffer(vk::Device device, FramebufferData&& data) : m_device{device}, m_data{std::move(data)} {}
}  // namespace MRG
