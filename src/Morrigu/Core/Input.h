#ifndef MRG_CLASS_INPUT
#define MRG_CLASS_INPUT

#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"

#include <utility>

namespace MRG
{
	class Input
	{
	protected:
		Input() = default;

	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		[[nodiscard]] static bool isKeyDown(KeyCode key);
		[[nodiscard]] static bool isMouseButtonDown(MouseCode button);
		[[nodiscard]] static std::pair<float, float> getMousePosition();
		[[nodiscard]] static float getMouseX();
		[[nodiscard]] static float getMouseY();
	};
}  // namespace MRG

#endif