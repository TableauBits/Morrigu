#ifndef MRG_CLASS_SHADER
#define MRG_CLASS_SHADER

#include <string>

namespace MRG
{
	class Shader
	{
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		~Shader();

		void bind() const;
		void unbind() const;

	private:
		uint32_t m_rendererId;
	};
}  // namespace MRG

#endif