#ifndef MRG_CLASS_INPUT
#define MRG_CLASS_INPUT

#include <utility>

namespace MRG
{
	class Input
	{
	public:
		[[nodiscard]] static bool isKeyDown(int keyCode);
		[[nodiscard]] static bool isMouseButtonDown(int button);
		[[nodiscard]] static std::pair<float, float> getMousePosition();
		[[nodiscard]] static float getMouseX();
		[[nodiscard]] static float getMouseY();
	};
}  // namespace MRG

#endif