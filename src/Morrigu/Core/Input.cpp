#include "Input.h"

#include "Core/Application.h"

namespace MRG
{
	bool Input::isKeyDown(int keyCode)
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		const auto state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonDown(int button)
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		const auto state = glfwGetMouseButton(window, button);
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
		const auto [x, y] = getMousePosition();
		return x;
	}

	float Input::getMouseY()
	{
		const auto [x, y] = getMousePosition();
		return y;
	}
}  // namespace MRG