#include "Shader.h"

#include "Core/Core.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace MRG
{
	Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* vertexSrc = vertexShader.c_str();
		glShaderSource(vertexID, 1, &vertexSrc, 0);

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

		const GLchar* fragmentSrc = fragmentShader.c_str();
		glShaderSource(fragmentID, 1, &fragmentSrc, 0);

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

		m_rendererId = glCreateProgram();

		glAttachShader(m_rendererId, vertexID);
		glAttachShader(m_rendererId, fragmentID);
		glLinkProgram(m_rendererId);

		GLint isProgramLinked = 0;
		glGetProgramiv(m_rendererId, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked == GL_FALSE) {
			GLint logLength = 0;
			glGetProgramiv(m_rendererId, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetProgramInfoLog(m_rendererId, logLength, &logLength, infoLog.data());

			glDeleteProgram(m_rendererId);
			glDeleteShader(vertexID);
			glDeleteShader(fragmentID);

			MRG_ENGINE_ERROR("Could not link program: {0}", infoLog.data());
			MRG_ASSERT(false, "Program linking failed !");
			return;
		}

		glDetachShader(m_rendererId, vertexID);
		glDetachShader(m_rendererId, fragmentID);
	}

	Shader::~Shader() { glDeleteProgram(m_rendererId); }

	void Shader::bind() const { glUseProgram(m_rendererId); }
	void Shader::unbind() const { glUseProgram(0); }

	void Shader::uploadUniform(const char* name, const glm::mat4& matrix)
	{
		auto location = glGetUniformLocation(m_rendererId, name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}  // namespace MRG