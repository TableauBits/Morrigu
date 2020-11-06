#ifndef MRG_CLASS_DESCRIPTORALLOCATOR
#define MRG_CLASS_DESCRIPTORALLOCATOR

#include "Core/GLMIncludeHelper.h"
#include "Renderer/APIs/Vulkan/Textures.h"
#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"

#include <vector>

namespace MRG::Vulkan
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 viewProjection;
	};

	enum class DrawCallType
	{
		Quad = 0,
		TexturedQuad
	};

	struct SceneDrawCallInfo
	{
		SceneDrawCallInfo(DrawCallType drawType, glm::mat4 drawTransform, glm::vec4 drawColor)
		    : type(drawType), transform(drawTransform), tiling(1.f), color(drawColor)
		{}
		SceneDrawCallInfo(DrawCallType drawType, glm::mat4 drawTransform, Ref<MRG::Texture2D> tex, float drawTiling, glm::vec4 drawColor)
		    : type(drawType), transform(drawTransform),
		      texture({std::static_pointer_cast<Texture2D>(tex)->getImageView(), std::static_pointer_cast<Texture2D>(tex)->getSampler()}),
		      tiling(drawTiling), color(drawColor)
		{}

		DrawCallType type;
		glm::mat4 transform;
		std::optional<std::pair<VkImageView, VkSampler>> texture;
		float tiling;
		glm::vec4 color;
	};

	class DescriptorAllocator
	{
	public:
		void init(const MRG::Vulkan::WindowProperties* data);
		void shutdown()
		{
			for (const auto& pool : m_pools) vkDestroyDescriptorPool(m_data->device, pool, nullptr);

			m_pools.clear();
			m_descriptorSets.clear();
		}

		[[nodiscard]] std::vector<VkDescriptorSet>
		requestDescriptorSets(const std::vector<SceneDrawCallInfo>& sceneInfo, uint32_t imageIndex, Ref<Texture2D> defaultTexture);

		static const uint32_t MAX_POOL_SIZE = 50u;

	private:
		void createDescriptorPool();

		const MRG::Vulkan::WindowProperties* m_data;
		std::vector<VkDescriptorPool> m_pools;
		std::vector<std::vector<VkDescriptorSet>> m_descriptorSets;
	};
}  // namespace MRG::Vulkan

#endif