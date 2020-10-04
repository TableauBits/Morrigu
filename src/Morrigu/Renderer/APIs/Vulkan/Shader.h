#ifndef MRG_VULKAN_IMPL_SHADER
#define MRG_VULKAN_IMPL_SHADER

#include "Renderer/Shader.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace MRG::Vulkan
{
	class Shader : public MRG::Shader
	{
	public:
		Shader(const std::string& filePath);
		virtual ~Shader();
		void destroy() override;

		void bind() const override;
		void unbind() const override;

		void upload(const std::string& name, int value) override;
		void upload(const std::string& name, float value) override;
		void upload(const std::string& name, const glm::vec3& value) override;
		void upload(const std::string& name, const glm::vec4& value) override;
		void upload(const std::string& name, const glm::mat4& value) override;

		[[nodiscard]] const std::string& getName() const override { return m_name; }

		void uploadUniform(const std::string& name, int value);

		void uploadUniform(const std::string& name, float value);
		void uploadUniform(const std::string& name, const glm::vec2& value);
		void uploadUniform(const std::string& name, const glm::vec3& value);
		void uploadUniform(const std::string& name, const glm::vec4& value);

		void uploadUniform(const std::string& name, const glm::mat3& matrix);
		void uploadUniform(const std::string& name, const glm::mat4& matrix);

		VkShaderModule m_vertexShaderModule, m_fragmentShaderModule;

	private:
		std::string m_name;
	};
}  // namespace MRG::Vulkan

#endif