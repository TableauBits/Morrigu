#include "OrthoCameraController.h"

#include "Core/Input.h"
#include "Debug/Instrumentor.h"

namespace MRG
{
	OrthoCameraController::OrthoCameraController(float aspectRatio, bool rotation)
	    : m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * zoomFactor, m_aspectRatio * zoomFactor, -zoomFactor, zoomFactor),
	      isRotatable(rotation)
	{}

	void OrthoCameraController::onUpdate(Timestep ts)
	{
		MRG_PROFILE_FUNCTION();

		if (MRG::Input::isKeyDown(MRG_KEY_W)) {
			m_position.x += -std::sin(glm::radians(m_rotation)) * m_transationFactor * ts;
			m_position.y += std::cos(glm::radians(m_rotation)) * m_transationFactor * ts;
		}

		if (MRG::Input::isKeyDown(MRG_KEY_A)) {
			m_position.x -= std::cos(glm::radians(m_rotation)) * m_transationFactor * ts;
			m_position.y -= std::sin(glm::radians(m_rotation)) * m_transationFactor * ts;
		}

		if (MRG::Input::isKeyDown(MRG_KEY_S)) {
			m_position.x -= -std::sin(glm::radians(m_rotation)) * m_transationFactor * ts;
			m_position.y -= std::cos(glm::radians(m_rotation)) * m_transationFactor * ts;
		}

		if (MRG::Input::isKeyDown(MRG_KEY_D)) {
			m_position.x += std::cos(glm::radians(m_rotation)) * m_transationFactor * ts;
			m_position.y += std::sin(glm::radians(m_rotation)) * m_transationFactor * ts;
		}

		m_camera.setPosition(m_position);

		if (isRotatable) {
			if (MRG::Input::isKeyDown(MRG_KEY_Q))
				m_rotation -= rotationSpeed * ts;

			if (MRG::Input::isKeyDown(MRG_KEY_E))
				m_rotation += rotationSpeed * ts;

			if (m_rotation > 180.f)
				m_rotation -= 360.f;
			if (m_rotation <= -180.f)
				m_rotation += 360.f;
			m_camera.setRotation(m_rotation);
		}

		m_transationFactor = zoomFactor * movementSpeed;
	}

	void OrthoCameraController::onEvent(Event& event)
	{
		MRG_PROFILE_FUNCTION();

		EventDispatcher dispatcher{event};
		dispatcher.dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& scrollEvent) { return onMouseScrolled(scrollEvent); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent) { return onWindowResized(resizeEvent); });
	}

	bool OrthoCameraController::onMouseScrolled(MouseScrolledEvent& event)
	{
		MRG_PROFILE_FUNCTION();

		zoomFactor -= event.getY() * 0.25f;
		zoomFactor = std::max(zoomFactor, 0.25f);
		m_camera.setProjection(-m_aspectRatio * zoomFactor, m_aspectRatio * zoomFactor, -zoomFactor, zoomFactor);

		return false;
	}

	bool OrthoCameraController::onWindowResized(WindowResizeEvent& event)
	{
		MRG_PROFILE_FUNCTION();

		m_aspectRatio = static_cast<float>(event.getWidth()) / static_cast<float>(event.getHeight());
		m_camera.setProjection(-m_aspectRatio * zoomFactor, m_aspectRatio * zoomFactor, -zoomFactor, zoomFactor);

		return false;
	}

}  // namespace MRG