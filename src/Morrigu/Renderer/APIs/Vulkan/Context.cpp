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
	static const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#ifdef MRG_DEBUG
	static constexpr bool enableValidation = true;
#else
	static constexpr bool enableValidation = false;
#endif

	// INITIALISATION
	[[nodiscard]] bool checkValidationLayerSupport()
	{
		MRG_TRACE("Validation layers requested. Checking availability...");
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto layerName : validationLayers) {
			bool layerFound = false;
			for (const auto& layer : availableLayers) {
				if (strcmp(layer.layerName, layerName) == 0) {
					layerFound = true;
					break;
				}
			}
			if (!layerFound) {
				MRG_ERROR("Validation layers missing !");
				return false;
			}
		}
		MRG_INFO("Validation layers found.");
		return true;
	}

	[[nodiscard]] std::vector<const char*> getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if constexpr (enableValidation) {
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return requiredExtensions;
	}

	[[nodiscard]] VkInstance createInstance(const char* appName)
	{
		// this if statement is split to allow constexpr if
		if constexpr (enableValidation) {
			if (!checkValidationLayerSupport())
				throw std::runtime_error("Validation layers requested but not found !");
		}

		VkInstance returnInstance;

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = "Morrigu";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
		appInfo.pApplicationName = appName;                     // TODO: verify if Vulkan copies the necessary information
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);  // TODO: give option to create appropriate versions
		appInfo.apiVersion = VK_API_VERSION_1_0;                // TODO: check if update may be necessary

		auto requiredExtensions = getRequiredExtensions();

		uint32_t supportedExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
		std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

		MRG_INFO("The following {} vulkan extensions are required:", requiredExtensions.size());
		for (std::size_t i = 0; i < requiredExtensions.size(); ++i) {
			bool isSupported = false;
			for (uint32_t j = 0; j < supportedExtensionCount; ++j) {
				if (strcmp(supportedExtensions[j].extensionName, requiredExtensions[i]) == 0) {
					MRG_INFO("\t{} (version {}) [SUPPORTED]", supportedExtensions[j].extensionName, supportedExtensions[j].specVersion);
					supportedExtensions.erase(supportedExtensions.begin() + j);
					--j;  // might underflow if set to 0, but will overflow back to 0
					isSupported = true;
					break;
				}
			}
			if (!isSupported) {
				MRG_ERROR("\t{} (NO VERSION FOUND) [NOT SUPPORTED]", requiredExtensions[i]);
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
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		if constexpr (enableValidation) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		MRG_VKVALIDATE(vkCreateInstance(&createInfo, nullptr, &returnInstance), "instance creation failed !");
		return returnInstance;
	}

	// CLEANUP

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
			m_instance = createInstance(data->title.c_str());
		} catch (std::runtime_error e) {
			MRG_ERROR("Vulkan error detected: {}", e.what());
		}
	}

	Context::~Context() { vkDestroyInstance(m_instance, nullptr); }

	void Context::swapBuffers()
	{
		// MRG_PROFILE_FUNCTION();
	}

	void Context::swapInterval(int) {}
}  // namespace MRG::Vulkan