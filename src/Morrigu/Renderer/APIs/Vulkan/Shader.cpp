#include "Shader.h"

#include "Core/Core.h"
#include "Debug/Instrumentor.h"
#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/APIs/Vulkan/WindowProperties.h"
#include "Renderer/Renderer2D.h"

#include <filesystem>
#include <fstream>
#include <ios>

namespace
{
	[[nodiscard]] std::vector<char> readFile(const std::string& fileName)
	{
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Could not open file !");

		std::size_t fileSize = static_cast<std::size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	[[nodiscard]] VkShaderModule createShader(const std::vector<char>& code, const VkDevice device)
	{
		VkShaderModule returnShader;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		MRG_VKVALIDATE(vkCreateShaderModule(device, &createInfo, nullptr, &returnShader), "failed to create shader!");
		return returnShader;
	}
}  // namespace

namespace MRG::Vulkan
{
	Shader::Shader(const std::string& filePath)
	{
		MRG_PROFILE_FUNCTION();

		std::filesystem::path shaderDir{filePath};
		std::filesystem::path vertFile{filePath + "/vert.spv"};
		std::filesystem::path fragFile{filePath + "/frag.spv"};
		MRG_CORE_ASSERT(std::filesystem::exists(shaderDir), fmt::format("Directory '{}' does not exist!", filePath));
		MRG_CORE_ASSERT(std::filesystem::is_directory(shaderDir),
		                fmt::format("Specified path '{}' doesn't reference a directory!", filePath));
		MRG_CORE_ASSERT(std::filesystem::exists(vertFile) && std::filesystem::exists(fragFile),
		                fmt::format("Directory '{}' does not contain vulkan shader files!", filePath));
		m_name = shaderDir.stem().string();

		const auto vertShaderSrc = readFile(vertFile.string());
		const auto fragShaderSrc = readFile(fragFile.string());

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		m_vertexShaderModule = createShader(vertShaderSrc, data->device);
		m_fragmentShaderModule = createShader(fragShaderSrc, data->device);
	}

	Shader::~Shader() { destroy(); }

	void Shader::destroy()
	{
		MRG_PROFILE_FUNCTION();

		if (m_isDestroyed)
			return;

		const auto data = static_cast<WindowProperties*>(glfwGetWindowUserPointer(Renderer2D::getGLFWWindow()));

		vkDestroyShaderModule(data->device, m_vertexShaderModule, nullptr);
		vkDestroyShaderModule(data->device, m_fragmentShaderModule, nullptr);

		m_isDestroyed = true;
	}

	void Shader::bind() const {}
	void Shader::unbind() const {}

	void Shader::upload(const std::string&, int) {}
	void Shader::upload(const std::string&, float) {}
	void Shader::upload(const std::string&, const glm::vec3&) {}
	void Shader::upload(const std::string&, const glm::vec4&) {}
	void Shader::upload(const std::string&, const glm::mat4&) {}

	void Shader::uploadUniform(const std::string&, int) {}

	void Shader::uploadUniform(const std::string&, float) {}
	void Shader::uploadUniform(const std::string&, const glm::vec2&) {}
	void Shader::uploadUniform(const std::string&, const glm::vec3&) {}
	void Shader::uploadUniform(const std::string&, const glm::vec4&) {}

	void Shader::uploadUniform(const std::string&, const glm::mat3&) {}
	void Shader::uploadUniform(const std::string&, const glm::mat4&) {}
}  // namespace MRG::Vulkan