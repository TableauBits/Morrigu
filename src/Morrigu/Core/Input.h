//
// Created by Mathis Lamidey on 2021-10-17.
//

#ifndef MORRIGU_INPUT_H
#define MORRIGU_INPUT_H

#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"
#include "Utils/GLMIncludeHelper.h"

#include <GLFW/glfw3.h>

namespace MRG::Input
{
	[[nodiscard]] bool isKeyPressed(GLFWwindow* window, KeyCode key);
	[[nodiscard]] bool isMouseButtonPressed(GLFWwindow* window, MouseCode mouseButton);
	[[nodiscard]] glm::vec2 getMousePosition(GLFWwindow* window);
}  // namespace MRG::Input

#endif
