#include "Input.h"

#include "Core/Application.h"

namespace MRG
{
	bool Input::isKeyDown(KeyCode key)
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		const auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonDown(MouseCode button)
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		const auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::getMousePosition()
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return std::pair{static_cast<float>(xPos), static_cast<float>(yPos)};
	}

	float Input::getMouseX()
	{
		float x;
		std::tie(x, std::ignore) = getMousePosition();
		return x;
	}

	float Input::getMouseY()
	{
		float y;
		std::tie(std::ignore, y) = getMousePosition();
		return y;
	}
}  // namespace MRG