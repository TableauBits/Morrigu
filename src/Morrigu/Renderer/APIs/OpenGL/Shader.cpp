#include "Shader.h"

#include "Core/Core.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace MRG::OpenGL
{
	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* vertexShader = vertexSrc.c_str();
		glShaderSource(vertexID, 1, &vertexShader, 0);

		glCompileShader(vertexID);

		GLint isShaderCompiled = 0;
		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &isShaderCompiled);
		if (isShaderCompiled == GL_FALSE) {
			GLint logLength = 0;
			glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetShaderInfoLog(vertexID, logLength, &logLength, infoLog.data());

			glDeleteShader(vertexID);

			MRG_ENGINE_ERROR("Could not compile shader: {0}", infoLog.data());
			MRG_ASSERT(false, "Vertex compilation failed !");
			return;
		}

		GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* fragmentShader = fragmentSrc.c_str();
		glShaderSource(fragmentID, 1, &fragmentShader, 0);

		glCompileShader(fragmentID);

		glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &isShaderCompiled);
		if (isShaderCompiled == GL_FALSE) {
			GLint logLength = 0;
			glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetShaderInfoLog(fragmentID, logLength, &logLength, infoLog.data());

			glDeleteShader(fragmentID);

			MRG_ENGINE_ERROR("Could not compile shader: {0}", infoLog.data());
			MRG_ASSERT(false, "Vertex compilation failed !");
			return;
		}

		m_rendererID = glCreateProgram();

		glAttachShader(m_rendererID, vertexID);
		glAttachShader(m_rendererID, fragmentID);
		glLinkProgram(m_rendererID);

		GLint isProgramLinked = 0;
		glGetProgramiv(m_rendererID, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked == GL_FALSE) {
			GLint logLength = 0;
			glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetProgramInfoLog(m_rendererID, logLength, &logLength, infoLog.data());

			glDeleteProgram(m_rendererID);
			glDeleteShader(vertexID);
			glDeleteShader(fragmentID);

			MRG_ENGINE_ERROR("Could not link program: {0}", infoLog.data());
			MRG_ASSERT(false, "Program linking failed !");
			return;
		}

		glDetachShader(m_rendererID, vertexID);
		glDetachShader(m_rendererID, fragmentID);
	}

	Shader::~Shader() { glDeleteProgram(m_rendererID); }

	void Shader::bind() const { glUseProgram(m_rendererID); }
	void Shader::unbind() const { glUseProgram(0); }

	void Shader::uploadUniform(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, value);
	}

	void Shader::uploadUniform(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, value);
	}

	void Shader::uploadUniform(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void Shader::uploadUniform(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::uploadUniform(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::uploadUniform(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::uploadUniform(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}  // namespace MRG::OpenGL