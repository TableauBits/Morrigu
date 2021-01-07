#include "SceneCamera.h"

namespace MRG
{
	SceneCamera::SceneCamera() { recalculateProjection(); }

	void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Orthographic;

		m_orthographicSize = size;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;
		recalculateProjection();
	}

	void SceneCamera::setPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Perspective;

		m_perspectiveFOV = verticalFOV;
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;
		recalculateProjection();
	}

	void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
	{
		m_aspectRatio = (float)width / (float)height;
		recalculateProjection();
	}

	void SceneCamera::recalculateProjection()
	{
		if (m_projectionType == ProjectionType::Orthographic) {
			float orthoLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoRight = m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoBottom = -m_orthographicSize * 0.5f;
			float orthoTop = m_orthographicSize * 0.5f;

			setProjection(glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar));
		} else {
			setProjection(glm::perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar));
		}
	}
}  // namespace MRG
