#ifndef MRG_VULKAN_IMPL_CONTEXT
#define MRG_VULKAN_IMPL_CONTEXT

#include "Renderer/Context.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace MRG::Vulkan
{
	class Context : public MRG::Context
	{
	public:
		Context(GLFWwindow* window);
		virtual ~Context();

		void swapBuffers() override;
		void swapInterval(int interval) override;

	private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_messenger;
		VkSurfaceKHR m_surface;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_device;
		VkQueue m_graphicsQueue;
	};
}  // namespace MRG::Vulkan

#endif