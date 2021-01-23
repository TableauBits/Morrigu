#ifndef MRG_OPENGL_IMPL_SHADER
#define MRG_OPENGL_IMPL_SHADER

#include "Core/GLMIncludeHelper.h"
#include "Renderer/Shader.h"

#include <glad/glad.h>

#include <cstdint>

namespace MRG::OpenGL
{
	class Shader : public MRG::Shader
	{
	public:
		Shader(const std::string& filePath, MRG::Encoding encoding);
		~Shader() override;
		void destroy() override;

		void bind() const override;
		void unbind() const override;

		void upload(const std::string& name, int value) override;
		void upload(const std::string& name, int* value, std::size_t count) override;
		void upload(const std::string& name, float value) override;
		void upload(const std::string& name, const glm::vec3& value) override;
		void upload(const std::string& name, const glm::vec4& value) override;
		void upload(const std::string& name, const glm::mat4& value) override;

		[[nodiscard]] const std::string& getName() const override { return m_name; }

		void uploadUniform(const std::string& name, int value) const;
		void uploadUniform(const std::string& name, int* value, std::size_t count) const;

		void uploadUniform(const std::string& name, float value) const;
		void uploadUniform(const std::string& name, const glm::vec2& value) const;
		void uploadUniform(const std::string& name, const glm::vec3& value) const;
		void uploadUniform(const std::string& name, const glm::vec4& value) const;

		void uploadUniform(const std::string& name, const glm::mat3& matrix) const;
		void uploadUniform(const std::string& name, const glm::mat4& matrix) const;

	private:
		[[nodiscard]] static std::string readFile(const std::string& filePath);
		[[nodiscard]] std::unordered_map<GLenum, std::string> preProcess(const std::string& source, Encoding encoding);
		void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		uint32_t m_rendererID{0};
		std::string m_name;
	};
}  // namespace MRG::OpenGL

#endif
