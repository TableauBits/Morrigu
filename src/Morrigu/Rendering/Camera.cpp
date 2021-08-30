//
// Created by Mathis Lamidey on 2021-05-16.
//

#include "Camera.h"

namespace MRG
{
	void StandardCamera::setPerspective(float fov, float nearClip, float farClip)
	{
		projectionType = ProjectionType::Perspective;

		pFOV  = fov;
		pNear = nearClip;
		pFar  = farClip;
	}

	void StandardCamera::setOrthgraphic(float size, float nearClip, float farClip)
	{
		projectionType = ProjectionType::Orthographic;

		oSize = size;
		oNear = nearClip;
		oFar  = farClip;
	}

	bool StandardCamera::onResize(const WindowResizeEvent& resizeEvent)
	{
		if (resizeEvent.getWidth() != 0 && resizeEvent.getHeight() != 0) {
			aspectRatio = static_cast<float>(resizeEvent.getWidth()) / static_cast<float>(resizeEvent.getHeight());
			recalculateViewProjection();
		}

		return false;
	}

	glm::quat StandardCamera::calculateOrientation() const { return glm::quat{glm::vec3{-pitch, -yaw, 0.f}}; }

	void StandardCamera::recalculateProjection()
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

			m_projection = glm::ortho(left, right, bottom, top, oNear, oFar);
		} break;
		}

		m_projection[1][1] *= -1;
	}

	void StandardCamera::recalculateView()
	{
		m_view = glm::translate(glm::identity<glm::mat4>(), position) * glm::toMat4(calculateOrientation());
		m_view = glm::inverse(m_view);
	}

	void StandardCamera::recalculateViewProjection()
	{
		recalculateProjection();
		recalculateView();
		m_viewProjection = m_projection * m_view;
	}

	bool PixelPerfectCamera::onResize(const WindowResizeEvent& resizeEvent)
	{
		if (resizeEvent.getWidth() != 0 && resizeEvent.getHeight() != 0) {
			width  = static_cast<float>(resizeEvent.getWidth());
			height = static_cast<float>(resizeEvent.getHeight());
			recalculateViewProjection();
		}

		return false;
	}

	// There is no use for the view matrix in the pixel perfect camera
	void PixelPerfectCamera::recalculateView() {}
	void PixelPerfectCamera::recalculateProjection()
	{
		m_projection = glm::ortho(0.f, width, 0.f, height, near, far);
		m_projection[1][1] *= -1;
	}
	void PixelPerfectCamera::recalculateViewProjection()
	{
		recalculateProjection();
		m_viewProjection = m_projection;
	}
}  // namespace MRG
