#include "OrthoCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace MRG
{
	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
	    : m_projectionMatrix(glm::ortho(left, right, bottom, top)), m_viewMatrix(1.0f)
	{
		m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthoCamera::setPosition(const glm::vec3& position)
	{
		m_position = position;
		recalculateViewMatrix();
	}

	void OrthoCamera::setRotation(float rotation)
	{
		m_rotation = rotation;
		recalculateViewMatrix();
	}

	void OrthoCamera::recalculateViewMatrix()
	{
		glm::mat4 transform =
		  glm::translate(glm::mat4(1.0f), m_position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

		m_viewMatrix = glm::inverse(transform);
		m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	}
}  // namespace MRG