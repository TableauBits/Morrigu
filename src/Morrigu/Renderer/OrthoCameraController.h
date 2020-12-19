#ifndef MRG_CLASS_CAMERACONTROLLER
#define MRG_CLASS_CAMERACONTROLLER

#include "Core/GLMIncludeHelper.h"
#include "Core/Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Renderer/OrthoCamera.h"

namespace MRG
{
	class OrthoCameraController
	{
	public:
		OrthoCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& event);
		void onResize(float width, float height);

		[[nodiscard]] OrthoCamera& getCamera() { return m_camera; }
		[[nodiscard]] const OrthoCamera& getCamera() const { return m_camera; }

		float zoomFactor = 1.f;
		float movementSpeed = 1.f, rotationSpeed = 180.f;

		bool isRotatable;

	private:
		bool onMouseScrolled(MouseScrolledEvent& event);
		bool onWindowResized(WindowResizeEvent& event);

		float m_aspectRatio;
		OrthoCamera m_camera;

		glm::vec3 m_position = {0.f, 0.f, 0.f};
		float m_rotation = 0.f;
		float m_transationFactor = 5.f;
	};

}  // namespace MRG

#endif
