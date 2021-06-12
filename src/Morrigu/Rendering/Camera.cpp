//
// Created by Mathis Lamidey on 2021-05-16.
//

#include "Camera.h"

namespace MRG
{
	void Camera::setPerspective(float fov, float nearClip, float farClip)
	{
		projectionType = ProjectionType::Perspective;

		pFOV  = fov;
		pNear = nearClip;
		pFar  = farClip;
	}

	void Camera::setOrthgraphic(float size, float nearClip, float farClip)
	{
		projectionType = ProjectionType::Orthographic;

		oSize = size;
		oNear = nearClip;
		oFar  = farClip;
	}

	bool Camera::onResize(const WindowResizeEvent& resizeEvent)
	{
		if (resizeEvent.getWidth() != 0 && resizeEvent.getHeight() != 0) {
			aspectRatio = static_cast<float>(resizeEvent.getWidth()) / static_cast<float>(resizeEvent.getHeight());
			recalculateViewProjection();
		}

		return false;
	}

	glm::quat Camera::calculateOrientation() const { return glm::quat{glm::vec3{-pitch, -yaw, 0.f}}; }

	void Camera::recalculateProjection()
	{
		switch (projectionType) {
		case ProjectionType::Perspective: {
			m_projection = glm::perspective(pFOV, aspectRatio, pNear, pFar);
		} break;
		case ProjectionType::Orthographic: {
			float left   = -oSize * aspectRatio * 0.5f;
			float right  = oSize * aspectRatio * 0.5f;
			float bottom = -oSize * 0.5f;
			float top    = oSize * 0.5f;

			m_projection = glm::ortho(left, right, bottom, top);
		} break;
		}

		m_projection[1][1] *= -1;
	}

	void Camera::recalculateView()
	{
		m_view = glm::translate(glm::identity<glm::mat4>(), position) * glm::toMat4(calculateOrientation());
		m_view = glm::inverse(m_view);
	}

	void Camera::recalculateViewProjection()
	{
		recalculateProjection();
		recalculateView();
		m_viewProjection = m_projection * m_view;
	}
}  // namespace MRG
