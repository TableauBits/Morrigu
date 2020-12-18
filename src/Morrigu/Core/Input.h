#ifndef MRG_CLASS_INPUT
#define MRG_CLASS_INPUT

#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"

#include <utility>

namespace MRG
{
	class Input
	{
	public:
		[[nodiscard]] static bool isKeyPressed(KeyCode key);

		[[nodiscard]] static bool isMouseButtonPressed(MouseCode button);
		[[nodiscard]] static std::pair<float, float> getMousePosition();
		[[nodiscard]] static float getMouseX();
		[[nodiscard]] static float getMouseY();
	};
}  // namespace MRG

#endif