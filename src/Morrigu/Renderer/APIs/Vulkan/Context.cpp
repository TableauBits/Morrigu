#include "Context.h"

#include "Core/Core.h"
#include "Core/Window.h"
#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

// This anonymous namespace will host all necessary vulkan calls to create the vulkan context
// These function will mostly follow the structure of https://vulkan-tutorial.com
namespace
{
	auto createInstance(const char* appName)
	{
		VkInstance returnInstance;

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = "Morrigu";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.pApplicationName = appName;                     // TODO: verify if Vulkan copies the necessary information
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);  // TODO: give option to create appropriate versions
		appInfo.apiVersion = VK_API_VERSION_1_0;                // TODO: check if update may be necessary

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;  // TODO: Change this when getting back to layers (validation especially)

		MRG_VKVALIDATE(vkCreateInstance(&createInfo, nullptr, &returnInstance), "instance creation failed !");
	}
}  // namespace

namespace MRG::Vulkan
{
	Context::Context([[maybe_unused]] GLFWwindow* window)
	{
		MRG_PROFILE_FUNCTION();

		MRG_CORE_ASSERT(window, "Window handle is null !");

		MRG_ENGINE_INFO("Using Vulkan as a rendering API. Other useful stats coming (hopefully) soon!");

		try {
			auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
			createInstance(data->title.c_str());
		} catch (std::runtime_error e) {
			MRG_ERROR("Vulkan error detected: {}", e.what());
		}
	}

	Context::~Context() {}

	void Context::swapBuffers()
	{
		// MRG_PROFILE_FUNCTION();
	}

	void Context::swapInterval(int) {}
}  // namespace MRG::Vulkan