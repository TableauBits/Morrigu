#include "Shader.h"

namespace MRG::Vulkan
{
	Shader::Shader(const std::string&, MRG::Encoding) {}

	Shader::Shader(const std::string& name, const std::string&, const std::string&) : m_name(name) {}

	Shader::~Shader() {}

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

	[[nodiscard]] std::string Shader::readFile(const std::string&) { return {}; }
	[[nodiscard]] std::unordered_map<uint64_t, std::string> Shader::preProcess(const std::string&, Encoding) { return {}; }
	void Shader::compile(const std::unordered_map<uint64_t, std::string>&) {}
}  // namespace MRG::Vulkan