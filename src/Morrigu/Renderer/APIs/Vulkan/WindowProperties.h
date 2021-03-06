#ifndef MRG_VULKAN_IMPL_WINDOWPROPERTIES
#define MRG_VULKAN_IMPL_WINDOWPROPERTIES

#include "Renderer/APIs/Vulkan/Pipeline.h"
#include "Renderer/APIs/Vulkan/VertexArray.h"
#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"
#include "Renderer/WindowProperties.h"

#include <string>
#include <vector>

namespace MRG::Vulkan
{
	class WindowProperties : public MRG::WindowProperties
	{
	public:
		WindowProperties(const char* newTitle, uint32_t newWidth, uint32_t newHeight, bool newVSync)
		    : MRG::WindowProperties{newTitle, newWidth, newHeight, newVSync}
		{}

		VkInstance instance{};
		VkDebugUtilsMessengerEXT messenger{};
		VkSurfaceKHR surface{};
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device{};
		Queue graphicsQueue{}, presentQueue{};
		SwapChain swapChain;
		VkDescriptorSetLayout descriptorSetLayout{};
		Pipeline clearingPipeline;
		Pipeline renderingPipeline;
		VkRenderPass ImGuiRenderPass{};
		VkCommandPool commandPool{};
		std::vector<std::array<VkCommandBuffer, 3>> commandBuffers;
		std::size_t currentFrame = 0;
		VkPushConstantRange pushConstantRanges{};
		VkDescriptorPool ImGuiPool{};
		Ref<Shader> textureShader;
		Ref<VertexArray> vertexArray;
	};
}  // namespace MRG::Vulkan

#endif
