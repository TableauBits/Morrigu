#include "Shader.h"

#include "Core/Core.h"
#include "Core/Warnings.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_NONSTANDARD_EXTENSION
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNING_POP

#include <array>
#include <filesystem>
#include <fstream>
#include <ios>

namespace
{
	[[nodiscard]] static GLenum shaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		MRG_CORE_ASSERT(false, fmt::format("Invalid shader type '{}'!", type));
		return 0;
	}

	[[nodiscard]] constexpr static const char* returnCharFromEncoding(MRG::Encoding encoding)
	{
		switch (encoding) {
		case MRG::Encoding::LF:
			return "\n";
		case MRG::Encoding::CRLF:
			return "\r\n";
		case MRG::Encoding::CR:
			return "\r";

		default:
			MRG_CORE_ASSERT(false, "Invalid encoding selected !");
			return "\0";
		}
	}
}  // namespace

namespace MRG::OpenGL
{
	Shader::Shader(const std::string& filePath, MRG::Encoding encoding)
	{
		std::filesystem::path path{filePath};
		MRG_CORE_ASSERT(std::filesystem::exists(path), fmt::format("File '{}' does not exist !", filePath));
		m_name = path.stem().string();

		const auto source = readFile(filePath);
		const auto shaderSources = preProcess(source, encoding);
		compile(shaderSources);
	}

	Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		compile(sources);
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

	std::string Shader::readFile(const std::string& filePath)
	{
		std::string result;
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		if (file.fail()) {
			MRG_ENGINE_ERROR("Could not open file '{}'", filePath);
			return result;
		}
		file.seekg(0, std::ios::end);
		result.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());
		file.close();

		return result;
	}

	std::unordered_map<GLenum, std::string> Shader::preProcess(const std::string& source, Encoding encoding)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		auto newLineChar = returnCharFromEncoding(encoding);

		constexpr const char* typeToken = "#type";
		static std::size_t typeTokenLength = strlen(typeToken);
		std::size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) {
			std::size_t eol = source.find_first_of(newLineChar, pos);
			MRG_CORE_ASSERT(eol != std::string::npos, "Invalid syntax in shader !");
			std::size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			MRG_CORE_ASSERT(shaderTypeFromString(type),
			                fmt::format("Invalid shader type '{}' in shader preprocessing type definition !", type));

			std::size_t nextLine = source.find_first_not_of(newLineChar, eol);
			MRG_CORE_ASSERT(nextLine != std::string::npos, "Syntax Error");
			pos = source.find(typeToken, nextLine);

			shaderSources[shaderTypeFromString(type)] =
			  (pos == std::string::npos) ? source.substr(nextLine) : source.substr(nextLine, pos - nextLine);
		}

		return shaderSources;
	}

	void Shader::compile(const std::unordered_map<GLenum, std::string>& sources)
	{
		auto program = glCreateProgram();
		MRG_CORE_ASSERT(sources.size() <= 4, "Invalid number of shaders in file");
		std::array<GLuint, 4> shaderIDs{};

		auto shaderIndex = 0;
		for (auto& [type, source] : sources) {
			auto shader = glCreateShader(type);
			const auto sourceCString = source.c_str();

			glShaderSource(shader, 1, &sourceCString, 0);
			glCompileShader(shader);

			GLint isShaderCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
			if (isShaderCompiled == GL_FALSE) {
				GLint logLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

				std::vector<GLchar> infoLog(logLength);
				glGetShaderInfoLog(shader, logLength, &logLength, infoLog.data());

				glDeleteShader(shader);

				MRG_ENGINE_ERROR("Could not compile shader: {0}", infoLog.data());
				MRG_ASSERT(false, "Shader compilation failed !");
				break;
			}
			glAttachShader(program, shader);
			shaderIDs[shaderIndex++] = shader;
		}
		m_rendererID = program;

		glLinkProgram(program);

		GLint isProgramLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked == GL_FALSE) {
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> infoLog(logLength);
			glGetProgramInfoLog(program, logLength, &logLength, infoLog.data());

			glDeleteProgram(program);
			for (auto id : shaderIDs) glDeleteShader(id);

			MRG_ENGINE_ERROR("Could not link program: {0}", infoLog.data());
			MRG_ASSERT(false, "Program linking failed !");
			return;
		}

		for (auto id : shaderIDs)
			if (id != 0) {
				glDetachShader(program, id);
				glDeleteShader(id);
			}
	}
}  // namespace MRG::OpenGL