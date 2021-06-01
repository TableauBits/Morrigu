//
// Created by Mathis Lamidey on 2021-05-16.
//

#ifndef MORRIGU_CAMERA_H
#define MORRIGU_CAMERA_H

#include "Events/ApplicationEvent.h"
#include "Utils/GLMIncludeHelper.h"

namespace MRG
{
	class Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective,
			Orthographic
		};

		[[nodiscard]] const glm::mat4& getProjection() const { return m_projection; }
		[[nodiscard]] const glm::mat4& getView() const { return m_view; }
		[[nodiscard]] const glm::mat4& getViewProjection() const { return m_viewProjection; };

		void setPerspective(float fov, float nearClip, float farClip);
		void setOrthgraphic(float size, float nearClip, float farClip);
		bool onResize(const WindowResizeEvent& resizeEvent);

		[[nodiscard]] glm::quat calculateOrientation() const;
		void recalculateProjection();
		void recalculateView();
		void recalculateViewProjection();

		ProjectionType projectionType = ProjectionType::Perspective;

		/// perspective data
		float pFOV  = glm::radians(45.f);
		float pNear = 0.1f;
		float pFar  = 1000.f;

		/// orographic data
		float oSize = 10.f;
		float oNear = -1.f;
		float oFar  = 1.f;

		/// common data
		float aspectRatio = 0.f;
		float pitch       = 0.f;
		float yaw         = 0.f;
		glm::vec3 position{};

	private:
		glm::mat4 m_projection{1.f};
		glm::mat4 m_view{1.f};
		glm::mat4 m_viewProjection{1.f};
	};
}  // namespace MRG

#endif  // MORRIGU_CAMERA_H
