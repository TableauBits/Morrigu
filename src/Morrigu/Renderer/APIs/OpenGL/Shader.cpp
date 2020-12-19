#include "Shader.h"

#include "Core/Core.h"
#include "Core/GLMIncludeHelper.h"
#include "Core/Warnings.h"
#include "Debug/Instrumentor.h"

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

	[[nodiscard]] static std::string shaderTypeFromEnum(GLenum type)
	{
		if (type == GL_VERTEX_SHADER)
			return "GL_VERTEX_SHADER";
		if (type == GL_FRAGMENT_SHADER)
			return "GL_FRAGMENT_SHADER";

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
		MRG_PROFILE_FUNCTION();

		std::filesystem::path shaderDir{filePath};
		std::filesystem::path shaderFile{filePath + "/gl.glsl"};
		MRG_CORE_ASSERT(std::filesystem::exists(shaderDir), fmt::format("Directory '{}' does not exist!", filePath));
		MRG_CORE_ASSERT(std::filesystem::is_directory(shaderDir),
		                fmt::format("Specified path '{}' doesn't reference a directory!", filePath));
		MRG_CORE_ASSERT(std::filesystem::exists(shaderFile), fmt::format("Directory '{}' does not contain gl shader file!", filePath));
		m_name = shaderDir.stem().string();

		const auto source = readFile(shaderFile.string());
		const auto shaderSources = preProcess(source, encoding);
		compile(shaderSources);
	}

	Shader::~Shader() { destroy(); }

	void Shader::destroy()
	{
		MRG_PROFILE_FUNCTION();

		if (m_isDestroyed)
			return;

		glDeleteProgram(m_rendererID);

		m_isDestroyed = true;
	}

	void Shader::bind() const
	{
		MRG_PROFILE_FUNCTION();

		glUseProgram(m_rendererID);
	}
	void Shader::unbind() const
	{
		MRG_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void Shader::upload(const std::string& name, int value) { uploadUniform(name, value); }
	void Shader::upload(const std::string& name, int* value, std::size_t count) { uploadUniform(name, value, count); }
	void Shader::upload(const std::string& name, float value) { uploadUniform(name, value); }
	void Shader::upload(const std::string& name, const glm::vec3& value) { uploadUniform(name, value); }
	void Shader::upload(const std::string& name, const glm::vec4& value) { uploadUniform(name, value); }
	void Shader::upload(const std::string& name, const glm::mat4& value) { uploadUniform(name, value); }

	void Shader::uploadUniform(const std::string& name, int value)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, value);
	}

	void Shader::uploadUniform(const std::string& name, int* value, std::size_t count)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(location, static_cast<uint32_t>(count), value);
	}

	void Shader::uploadUniform(const std::string& name, float value)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, value);
	}

	void Shader::uploadUniform(const std::string& name, const glm::vec2& value)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void Shader::uploadUniform(const std::string& name, const glm::vec3& value)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::uploadUniform(const std::string& name, const glm::vec4& value)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::uploadUniform(const std::string& name, const glm::mat3& matrix)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::uploadUniform(const std::string& name, const glm::mat4& matrix)
	{
		MRG_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	std::string Shader::readFile(const std::string& filePath)
	{
		MRG_PROFILE_FUNCTION();

		std::string result;
		std::ifstream file(filePath, std::ios::in | std::ios::binary);
		if (file.fail()) {
			MRG_ENGINE_ERROR("Could not open file '{}'", filePath);
			return result;
		}

		file.seekg(0, std::ios::end);
		const auto size = file.tellg();
		if (size == -1) {
			MRG_ENGINE_ERROR("Could not read file '{}'", filePath);
			return result;
		}

		result.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(&result[0], result.size());

		return result;
	}

	std::unordered_map<GLenum, std::string> Shader::preProcess(const std::string& source, Encoding encoding)
	{
		MRG_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;
		auto newLineChar = returnCharFromEncoding(encoding);

		constexpr const char* typeToken = "#type";
		static std::size_t typeTokenLength = strlen(typeToken);
		std::size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) {
			std::size_t eol = source.find_first_of(newLineChar, pos);
			MRG_CORE_ASSERT(eol != std::string::npos, fmt::format("Invalid syntax in shader '{}'!", m_name));
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
		MRG_PROFILE_FUNCTION();

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

				MRG_ENGINE_ERROR("Could not compile the {} of '{}': {}", shaderTypeFromEnum(type), m_name, infoLog.data());
				MRG_CORE_ASSERT(false, "Shader compilation failed !");
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
			MRG_CORE_ASSERT(false, "Program linking failed !");
			return;
		}

		for (auto id : shaderIDs)
			if (id != 0) {
				glDetachShader(program, id);
				glDeleteShader(id);
			}
	}
}  // namespace MRG::OpenGL
