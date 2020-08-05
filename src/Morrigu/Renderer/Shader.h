#ifndef MRG_CLASS_SHADER
#define MRG_CLASS_SHADER

#include <string>

namespace MRG
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		[[nodiscard]] static Shader* create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}  // namespace MRG

#endif