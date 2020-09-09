#include "Context.h"

#include "Core/Core.h"
#include "Core/Window.h"
#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <vector>

// This anonymous namespace will host all necessary vulkan calls to create the vulkan context
// These function will mostly follow the structure of https://vulkan-tutorial.com
namespace
{
	[[nodiscard]] VkInstance createInstance(const char* appName)
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

		uint32_t supportedExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
		std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

		MRG_INFO("GLFW requires the following {} extensions for vulkan to work:", glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
			bool isSupported = false;
			for (uint32_t j = 0; j < supportedExtensionCount; ++j) {
				if (strcmp(supportedExtensions[j].extensionName, glfwExtensions[i]) == 0) {
					MRG_INFO("\t{} (version {}) [SUPPORTED]", supportedExtensions[j].extensionName, supportedExtensions[j].specVersion);
					supportedExtensions.erase(supportedExtensions.begin() + j);
					--j;  // might underflow if set to 0, but will overflow back to 0
					isSupported = true;
					break;
				}
			}
			if (!isSupported) {
				MRG_ERROR("\t{} (NO VERSION FOUND) [NOT SUPPORTED]", glfwExtensions[i]);
				MRG_ASSERT(false, "Required extension not supported by hardware !");
			}
		}
		MRG_TRACE("All required extensions have been found. Additional supported extensions ({}): ", supportedExtensions.size());
		for (const auto& extension : supportedExtensions) {
			MRG_TRACE("\t{} (version {})", extension.extensionName, extension.specVersion);
		}

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;  // TODO: Change this when getting back to layers (validation especially)

		MRG_VKVALIDATE(vkCreateInstance(&createInfo, nullptr, &returnInstance), "instance creation failed !");
		return returnInstance;
	}
}  // namespace

namespace MRG::Vulkan
{
	Context::Context(GLFWwindow* window)
	{
		MRG_PROFILE_FUNCTION();

		MRG_CORE_ASSERT(window, "Window handle is null !");

		MRG_ENGINE_INFO("Using Vulkan as a rendering API. Other useful stats will follow:");

		try {
			auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));
			[[maybe_unused]] const auto instance = createInstance(data->title.c_str());
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