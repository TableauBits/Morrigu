#ifndef MRG_CLASS_RENDERER
#define MRG_CLASS_RENDERER

namespace MRG
{
	enum class RendererAPI
	{
		None = 0,
		OpenGL = 1
	};

	class Renderer
	{
	public:
		inline static RendererAPI getAPI() { return s_API; }

	private:
		static const RendererAPI s_API;
	};
}  // namespace MRG

#endif