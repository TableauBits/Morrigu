#ifndef MRG_CLASS_PIPELINE
#define MRG_CLASS_PIPELINE

#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"

namespace MRG::Vulkan
{
	class Pipeline
	{
	public:
		Pipeline(MRG::Shader shader)

	private:
		VkPipeline m_handle;
	};
}  // namespace MRG::Vulkan

#endif
