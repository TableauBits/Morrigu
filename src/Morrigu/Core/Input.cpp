//
// Created by Mathis Lamidey on 2021-10-17.
//

#include "Input.h"

namespace MRG::Input
{
	bool isKeyPressed(GLFWwindow* window, KeyCode key)
	{
		const auto keyState = glfwGetKey(window, key);
		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool isMouseButtonPressed(GLFWwindow* window, MouseCode mouseButton)
	{
		const auto mouseState = glfwGetMouseButton(window, mouseButton);
		return mouseState == GLFW_PRESS;
	}

	glm::vec2 getMousePosition(GLFWwindow* window)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return {static_cast<float>(x), static_cast<float>(y)};
	}
}  // namespace MRG::Input
