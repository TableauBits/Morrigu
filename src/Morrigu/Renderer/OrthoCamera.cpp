#include "OrthoCamera.h"

#include "Debug/Instrumentor.h"
#include "Renderer/RenderingAPI.h"

namespace MRG
{
	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
	    : m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f)), m_viewMatrix(1.f), m_projectionViewMatrix(1.f)
	{
		MRG_PROFILE_FUNCTION()

		if (RenderingAPI::getAPI() == RenderingAPI::API::Vulkan)
			m_projectionMatrix[1][1] *= -1;

		m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthoCamera::setProjection(float left, float right, float bottom, float top)
	{
		MRG_PROFILE_FUNCTION()

		m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
		if (RenderingAPI::getAPI() == RenderingAPI::API::Vulkan)
			m_projectionMatrix[1][1] *= -1;

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
		MRG_PROFILE_FUNCTION()

		glm::mat4 transform =
		  glm::translate(glm::mat4(1.f), m_position) * glm::rotate(glm::mat4(1.f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

		m_viewMatrix = glm::inverse(transform);
		m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	}
}  // namespace MRG
