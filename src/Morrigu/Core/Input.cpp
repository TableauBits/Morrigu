#include "Input.h"

#include "Core/Application.h"

namespace MRG
{
	bool Input::isKeyPressed(KeyCode key)
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		const auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(MouseCode button)
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		const auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::getMousePosition()
	{
		const auto window = Application::get().getWindow().getGLFWWindow();
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return glm::vec2{static_cast<float>(xPos), static_cast<float>(yPos)};
	}

	float Input::getMouseX() { return getMousePosition().x; }

	float Input::getMouseY() { return getMousePosition().y; }
}  // namespace MRG
