#include "RenderingAPI.h"

#include "Debug/Instrumentor.h"

#include "vulkan/vulkan.hpp"

namespace MRG::Vulkan
{
	void RenderingAPI::init() {}

	void RenderingAPI::setViewport(uint32_t, uint32_t, uint32_t, uint32_t) {}
	void RenderingAPI::setClearColor(const glm::vec4&) {}
	void RenderingAPI::clear() {}

	void RenderingAPI::drawIndexed(const Ref<VertexArray>&) {}
}  // namespace MRG::Vulkan