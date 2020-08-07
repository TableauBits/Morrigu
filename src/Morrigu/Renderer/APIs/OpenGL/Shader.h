#ifndef MRG_OPENGL_IMPL_SHADER
#define MRG_OPENGL_IMPL_SHADER

#include "Renderer/Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstdint>

namespace MRG::OpenGL
{
	class Shader : public MRG::Shader
	{
	public:
		Shader(const std::string& filePath, MRG::Encoding encoding);
		Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader();

		void bind() const override;
		void unbind() const override;

		[[nodiscard]] const std::string& getName() const override { return m_name; }

		void uploadUniform(const std::string& name, int value);

		void uploadUniform(const std::string& name, float value);
		void uploadUniform(const std::string& name, const glm::vec2& value);
		void uploadUniform(const std::string& name, const glm::vec3& value);
		void uploadUniform(const std::string& name, const glm::vec4& value);

		void uploadUniform(const std::string& name, const glm::mat3& matrix);
		void uploadUniform(const std::string& name, const glm::mat4& matrix);

	private:
		[[nodiscard]] std::string readFile(const std::string& filePath);
		[[nodiscard]] std::unordered_map<GLenum, std::string> preProcess(const std::string& source, Encoding encoding);
		void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		uint32_t m_rendererID;
		std::string m_name;
	};
}  // namespace MRG::OpenGL

#endif