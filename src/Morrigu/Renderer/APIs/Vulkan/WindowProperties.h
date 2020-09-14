#ifndef MRG_VULKAN_IMPL_WINDOWPROPERTIES
#define MRG_VULKAN_IMPL_WINDOWPROPERTIES

#include "Renderer/WindowProperties.h"

#include <vulkan/vulkan.hpp>

namespace MRG::Vulkan
{
	class WindowProperties : public MRG::WindowProperties
	{
	public:
		WindowProperties(const std::string& newTitle, unsigned int newWidth, unsigned int newHeight, bool newVSync)
		    : MRG::WindowProperties{newTitle, newWidth, newHeight, newVSync}
		{}

		VkInstance instance;
		VkDebugUtilsMessengerEXT messenger;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue, presentQueue;
		VkSwapchainKHR swapChain;
	};
}  // namespace MRG::Vulkan

#endif