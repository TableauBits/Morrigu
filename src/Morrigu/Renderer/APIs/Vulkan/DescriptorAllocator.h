#ifndef MRG_CLASS_DESCRIPTORALLOCATOR
#define MRG_CLASS_DESCRIPTORALLOCATOR

#include "Core/GLMIncludeHelper.h"
#include "Renderer/APIs/Vulkan/Textures.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"

#include <vulkan/vulkan.hpp>

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
		SceneDrawCallInfo(DrawCallType type, glm::mat4 transform) : type(type), transform(transform) {}
		SceneDrawCallInfo(DrawCallType type, glm::mat4 transform, Ref<MRG::Texture2D> tex)
		    : type(type), transform(transform),
		      texture({std::static_pointer_cast<Texture2D>(tex)->getImageView(), std::static_pointer_cast<Texture2D>(tex)->getSampler()})
		{}

		DrawCallType type;
		glm::mat4 transform;
		std::optional<std::pair<VkImageView, VkSampler>> texture;
	};

	class DescriptorAllocator
	{
	public:
		void init(const MRG::Vulkan::WindowProperties* data);
		void shutdown()
		{
			for (const auto& pool : m_pools) vkDestroyDescriptorPool(m_data->device, pool, nullptr);
		}

		[[nodiscard]] std::vector<VkDescriptorSet> requestDescriptorSets(const std::vector<SceneDrawCallInfo>& sceneInfo,
		                                                                 uint32_t imageIndex);

		static const uint32_t MAX_POOL_SIZE = 50u;

	private:
		void createDescriptorPool();

		const MRG::Vulkan::WindowProperties* m_data;
		std::vector<VkDescriptorPool> m_pools;
		std::vector<std::vector<VkDescriptorSet>> m_descriptorSets;
	};
}  // namespace MRG::Vulkan

#endif