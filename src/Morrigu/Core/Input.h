#ifndef MRG_CLASS_INPUT
#define MRG_CLASS_INPUT

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

		[[nodiscard]] static bool isKeyDown(int keyCode);
		[[nodiscard]] static bool isMouseButtonDown(int button);
		[[nodiscard]] static std::pair<float, float> getMousePosition();
		[[nodiscard]] static float getMouseX();
		[[nodiscard]] static float getMouseY();
	};
}  // namespace MRG

#endif