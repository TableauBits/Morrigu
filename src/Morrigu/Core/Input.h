#ifndef MRG_CLASS_INPUT
#define MRG_CLASS_INPUT

#include <utility>

namespace MRG
{
	class Input
	{
	public:
		static bool isKeyDown(int keyCode);
		static bool isMouseButtonDown(int button);
		static std::pair<float, float> getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}  // namespace MRG

#endif