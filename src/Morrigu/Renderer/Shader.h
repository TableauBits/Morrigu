#ifndef MRG_CLASS_SHADER
#define MRG_CLASS_SHADER

#include "Core/Core.h"
#include "Core/GLMIncludeHelper.h"

#include <string>
#include <unordered_map>

namespace MRG
{
	enum class Encoding
	{
		LF = 0,
		CRLF,
		CR
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void destroy() = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void upload(const std::string& name, int value) = 0;
		virtual void upload(const std::string& name, int* value, std::size_t count) = 0;
		virtual void upload(const std::string& name, float value) = 0;
		virtual void upload(const std::string& name, const glm::vec3& value) = 0;
		virtual void upload(const std::string& name, const glm::vec4& value) = 0;
		virtual void upload(const std::string& name, const glm::mat4& value) = 0;

		[[nodiscard]] virtual const std::string& getName() const = 0;

		[[nodiscard]] static Ref<Shader> create(const std::string& filePath, Encoding encoding = Encoding::LF);

	protected:
		bool m_isDestroyed = false;
	};

	class ShaderLibrary
	{
	public:
		void add(const Ref<Shader>& shader);
		void add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> load(const std::string& filePath, Encoding encoding = Encoding::LF);
		Ref<Shader> load(const std::string& name, const std::string& filePath);

		[[nodiscard]] Ref<Shader> get(const std::string& name);
		[[nodiscard]] bool exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>> m_shaders;
	};
}  // namespace MRG

#endif