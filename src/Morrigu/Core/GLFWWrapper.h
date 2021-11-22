//
// Created by Mathis Lamidey on 2021-09-16.
//

#ifndef MORRIGU_GLFW_WRAPPER_H
#define MORRIGU_GLFW_WRAPPER_H

namespace MRG
{
	class GLFWWrapper
	{
	public:
		GLFWWrapper();
		~GLFWWrapper();

		GLFWWrapper(const GLFWWrapper&) = delete;
		GLFWWrapper(GLFWWrapper&&)      = delete;

		GLFWWrapper& operator=(const GLFWWrapper&) = delete;
		GLFWWrapper& operator=(GLFWWrapper&&) = delete;
	};
}  // namespace MRG

#endif
