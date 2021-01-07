#ifndef MRG_CLASS_EDITORCAMERA
#define MRG_CLASS_EDITORCAMERA

#include "Core/Timestep.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Renderer/Camera.h"

#include <utility>

namespace MRG
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void onUpdate(Timestep ts);
		void onEvent(Event& event);

		inline void setViewportSize(float width, float height)
		{
			m_viewportWidth = width;
			m_viewportHeight = height;
			updateProjection();
		}

		[[nodiscard]] const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		[[nodiscard]] glm::mat4 getViewProjection() const { return getProjection() * m_viewMatrix; }

		[[nodiscard]] glm::vec3 getUpDirection() const;
		[[nodiscard]] glm::vec3 getRightDirection() const;
		[[nodiscard]] glm::vec3 getForwardDirection() const;
		[[nodiscard]] const glm::vec3& getPosition() const { return m_position; }
		[[nodiscard]] glm::quat getOrientation() const;

		[[nodiscard]] float getPitch() const { return m_pitch; }
		[[nodiscard]] float getYaw() const { return m_yaw; }

		float distance{10.f};

	private:
		void updateProjection();
		void updateView();

		bool onMouseScroll(MouseScrolledEvent& event);

		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		[[nodiscard]] glm::vec3 calculatePosition() const;

		[[nodiscard]] std::pair<float, float> panSpeed() const;
		[[nodiscard]] static float rotationSpeed();
		[[nodiscard]] float zoomSpeed() const;

		float m_FOV{45.f}, m_aspectRatio{1.778f}, m_nearClip{0.1f}, m_farClip{1000.f};

		glm::mat4 m_viewMatrix{};
		glm::vec3 m_position{0.f, 0.f, 0.f};
		glm::vec3 m_focalPoint{0.f, 0.f, 0.f};

		glm::vec2 m_initialMousePosition{0.f, 0.f};

		float m_pitch{0.f}, m_yaw{0.f};

		float m_viewportWidth{1280.f}, m_viewportHeight{720.f};
	};
}  // namespace MRG

#endif  // MRG_CLASS_EditorCamera
