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
		virtual ~Camera() = default;

		[[nodiscard]] glm::mat4& getProjection() { return m_projection; }
		[[nodiscard]] glm::mat4& getView() { return m_view; }
		[[nodiscard]] glm::mat4& getViewProjection() { return m_viewProjection; };
		[[nodiscard]] const glm::mat4& getProjection() const { return m_projection; }
		[[nodiscard]] const glm::mat4& getView() const { return m_view; }
		[[nodiscard]] const glm::mat4& getViewProjection() const { return m_viewProjection; };

		virtual bool onResize(const WindowResizeEvent& resizeEvent) = 0;

		virtual void recalculateProjection()     = 0;
		virtual void recalculateView()           = 0;
		virtual void recalculateViewProjection() = 0;

		glm::vec3 position{};

	protected:
		glm::mat4 m_projection{1.f};
		glm::mat4 m_view{1.f};
		glm::mat4 m_viewProjection{1.f};
	};

	// A standard camera class that can be either perspective or orthographic camera
	class StandardCamera : public Camera
	{
	public:
		virtual ~StandardCamera() = default;

		enum class ProjectionType
		{
			Perspective,
			Orthographic
		};

		void setPerspective(float fov, float nearClip, float farClip);
		void setOrthgraphic(float size, float nearClip, float farClip);

		bool onResize(const WindowResizeEvent& resizeEvent) override;

		void recalculateProjection() override;
		void recalculateView() override;
		void recalculateViewProjection() override;
		[[nodiscard]] glm::quat calculateOrientation() const;
		[[nodiscard]] glm::vec3 getForwardVector() const;
		[[nodiscard]] glm::vec3 getRightVector() const;
		[[nodiscard]] glm::vec3 getUpVector() const;

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
	};

	// /!\ This camera class doesn't preserve aspect ratio, but provides a 1:1 mapping from pixels to coordinates.
	// This camera is orthographic ONLY. Mainly useful for UI elements.
	class PixelPerfectCamera : public Camera
	{
	public:
		virtual ~PixelPerfectCamera() = default;

		bool onResize(const WindowResizeEvent& resizeEvent) override;

		void recalculateProjection() override;
		void recalculateView() override;
		void recalculateViewProjection() override;

		float near = -1.f;
		float far  = 1.f;
		float width{};
		float height{};
	};
}  // namespace MRG

#endif  // MORRIGU_CAMERA_H
