#include "OrthoCameraController.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"

namespace MRG
{
	OrthoCameraController::OrthoCameraController(float aspectRatio, bool rotation)
	    : m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomFactor, m_aspectRatio * m_zoomFactor, -m_zoomFactor, m_zoomFactor),
	      m_isRotatable(rotation)
	{}

	void OrthoCameraController::onUpdate(Timestep ts)
	{
		if (MRG::Input::isKeyDown(MRG_KEY_W)) {
			m_position.x += -std::sin(glm::radians(m_rotation)) * m_movementSpeed * ts;
			m_position.y += std::cos(glm::radians(m_rotation)) * m_movementSpeed * ts;
		}

		if (MRG::Input::isKeyDown(MRG_KEY_A)) {
			m_position.x -= std::cos(glm::radians(m_rotation)) * m_movementSpeed * ts;
			m_position.y -= std::sin(glm::radians(m_rotation)) * m_movementSpeed * ts;
		}

		if (MRG::Input::isKeyDown(MRG_KEY_S)) {
			m_position.x -= -std::sin(glm::radians(m_rotation)) * m_movementSpeed * ts;
			m_position.y -= std::cos(glm::radians(m_rotation)) * m_movementSpeed * ts;
		}

		if (MRG::Input::isKeyDown(MRG_KEY_D)) {
			m_position.x += std::cos(glm::radians(m_rotation)) * m_movementSpeed * ts;
			m_position.y += std::sin(glm::radians(m_rotation)) * m_movementSpeed * ts;
		}

		m_camera.setPosition(m_position);

		if (m_isRotatable) {
			if (MRG::Input::isKeyDown(MRG_KEY_Q))
				m_rotation -= m_rotationSpeed * ts;

			if (MRG::Input::isKeyDown(MRG_KEY_E))
				m_rotation += m_rotationSpeed * ts;

			if (m_rotation > 180.f)
				m_rotation -= 360.f;
			if (m_rotation <= -180.f)
				m_rotation += 360.f;
			m_camera.setRotation(m_rotation);
		}

		m_movementSpeed = m_zoomFactor;
	}

	void OrthoCameraController::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& scrollEvent) { return onMouseScrolled(scrollEvent); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent) { return onWindowResized(resizeEvent); });
	}

	bool OrthoCameraController::onMouseScrolled(MouseScrolledEvent& event)
	{
		m_zoomFactor -= event.getY() * 0.25f;
		m_zoomFactor = std::max(m_zoomFactor, 0.25f);
		m_camera.setProjection(-m_aspectRatio * m_zoomFactor, m_aspectRatio * m_zoomFactor, -m_zoomFactor, m_zoomFactor);

		return false;
	}

	bool OrthoCameraController::onWindowResized(WindowResizeEvent& event)
	{
		m_aspectRatio = static_cast<float>(event.getWidth()) / static_cast<float>(event.getHeight());
		m_camera.setProjection(-m_aspectRatio * m_zoomFactor, m_aspectRatio * m_zoomFactor, -m_zoomFactor, m_zoomFactor);

		return false;
	}

}  // namespace MRG