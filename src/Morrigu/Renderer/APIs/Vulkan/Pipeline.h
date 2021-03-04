#ifndef MRG_CLASS_PIPELINE
#define MRG_CLASS_PIPELINE

#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"

namespace MRG::Vulkan
{
	struct PipelineSpec
	{
		Ref<Shader> shader;
		std::vector<VkFormat> colorFormats;
		VkFormat depthFormat;
		VkAttachmentDescription colorAttachmentDescription;
		VkAttachmentDescription depthAttachmentDescription;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	};

	class Pipeline
	{
	public:
		explicit Pipeline() {}
		explicit Pipeline(const PipelineSpec& specification);
		~Pipeline();

		void destroy();

		void init(const PipelineSpec& specification);

		[[nodiscard]] VkPipeline getHandle() const { return m_handle; }
		[[nodiscard]] VkRenderPass getRenderpass() const { return m_renderPass; }
		[[nodiscard]] VkPipelineLayout getLayout() const { return m_layout; }

	private:
		std::size_t initRenderpass(const PipelineSpec& specification);

		VkPipelineLayout m_layout{};
		VkRenderPass m_renderPass{};
		VkPipeline m_handle{};

		bool m_isDestroyed = true;
	};
}  // namespace MRG::Vulkan

#endif
