#ifndef MRG_CLASS_CAMERACONTROLLER
#define MRG_CLASS_CAMERACONTROLLER

#include "Core/Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "OrthoCamera.h"

#include <glm/glm.hpp>

namespace MRG
{
	class OrthoCameraController
	{
	public:
		OrthoCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& event);

		[[nodiscard]] OrthoCamera& getCamera() { return m_camera; }
		[[nodiscard]] const OrthoCamera& getCamera() const { return m_camera; }

	private:
		bool onMouseScrolled(MouseScrolledEvent& event);
		bool onWindowResized(WindowResizeEvent& event);

		float m_aspectRatio;
		float m_zoomFactor = 1.f;
		OrthoCamera m_camera;
		bool m_isRotatable;

		glm::vec3 m_position = {0.f, 0.f, 0.f};
		float m_rotation = 0.f;
		float m_movementSpeed = 5.f, m_rotationSpeed = 180.f;
	};

}  // namespace MRG

#endif