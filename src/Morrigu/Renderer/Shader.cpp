#include "Shader.h"

#include "Renderer/APIs/OpenGL/Shader.h"
#include "Renderer/APIs/Vulkan/Shader.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	Ref<Shader> Shader::create(const std::string& filePath, Encoding encoding)
	{
		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			return createRef<OpenGL::Shader>(filePath, encoding);
		}

		case RenderingAPI::API::Vulkan: {
			return createRef<Vulkan::Shader>(filePath);
		}

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION! ({})", RenderingAPI::getAPI()))
			return nullptr;
		}
		}
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
	{
		if (exists(name)) {
			MRG_WARN("Shader '{}' already exists!", name)
			return;
		}
		m_shaders[name] = shader;
	}

	void ShaderLibrary::add(const Ref<Shader>& shader) { add(shader->getName(), shader); }

	Ref<Shader> ShaderLibrary::load(const std::string& filePath, Encoding encoding)
	{
		auto shader = Shader::create(filePath, encoding);
		add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filePath)
	{
		auto shader = Shader::create(filePath);
		add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name)
	{
		if (!exists(name)) {
			MRG_WARN("Shader '{}' does not exist!", name)
			return nullptr;
		}
		return m_shaders[name];
	}

	bool ShaderLibrary::exists(const std::string& name) const { return m_shaders.find(name) != m_shaders.end(); }
}  // namespace MRG
