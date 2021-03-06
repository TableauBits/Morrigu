#include "EditorCamera.h"

#include "Core/Input.h"

namespace MRG
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
	    : Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)), m_FOV(fov), m_nearClip(nearClip), m_farClip(farClip)
	{
		updateView();
	}

	void EditorCamera::onUpdate(Timestep)
	{
		if (Input::isKeyPressed(Key::LeftAlt)) {
			const glm::vec2& mouse{Input::getMouseX(), Input::getMouseY()};
			glm::vec2 delta = (mouse - m_initialMousePosition) * 0.003f;
			m_initialMousePosition = mouse;

			if (Input::isMouseButtonPressed(Mouse::ButtonMiddle)) {
				mousePan(delta);
			} else if (Input::isMouseButtonPressed(Mouse::ButtonLeft)) {
				mouseRotate(delta);
			} else if (Input::isMouseButtonPressed(Mouse::ButtonRight)) {
				mouseZoom(delta.y);
			}
		}

		updateView();
	}
	void EditorCamera::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<MouseScrolledEvent>(
		  [this](MouseScrolledEvent& mouseScrolledEvent) -> bool { return onMouseScroll(mouseScrolledEvent); });
	}

	glm::vec3 EditorCamera::getUpDirection() const { return glm::rotate(getOrientation(), glm::vec3{0.f, 1.f, 0.f}); }
	glm::vec3 EditorCamera::getRightDirection() const { return glm::rotate(getOrientation(), glm::vec3{1.f, 0.f, 0.f}); }
	glm::vec3 EditorCamera::getForwardDirection() const { return glm::rotate(getOrientation(), glm::vec3{0.f, 0.f, -1.f}); }

	glm::quat EditorCamera::getOrientation() const { return glm::quat{glm::vec3{-m_pitch, -m_yaw, 0.f}}; }

	void EditorCamera::updateProjection()
	{
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		setProjection(glm::perspective(glm::radians(m_FOV), m_aspectRatio, m_nearClip, m_farClip));
	}

	void EditorCamera::updateView()
	{
		m_position = calculatePosition();
		const auto orientation = getOrientation();
		m_viewMatrix = glm::translate(glm::mat4{1.f}, m_position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

	bool EditorCamera::onMouseScroll(MouseScrolledEvent& event)
	{
		float delta = event.getY() * 0.1f;
		mouseZoom(delta);
		updateView();

		return false;
	}

	void EditorCamera::mousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = panSpeed();
		m_focalPoint -= getRightDirection() * delta.x * xSpeed * distance;
		m_focalPoint += getUpDirection() * delta.y * ySpeed * distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta)
	{
		auto yawSign = getUpDirection().y < 0 ? -1 : 1;
		m_yaw += yawSign * delta.x * rotationSpeed();
		m_pitch += delta.y * rotationSpeed();
	}

	void EditorCamera::mouseZoom(float delta)
	{
		distance -= delta * zoomSpeed();
		if (distance < 1.f) {
			m_focalPoint += getForwardDirection();
			distance = 1.f;
		}
	}

	glm::vec3 EditorCamera::calculatePosition() const { return m_focalPoint - getForwardDirection() * distance; }

	std::pair<float, float> EditorCamera::panSpeed() const
	{
		auto x = std::min(m_viewportWidth / 1000.f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;  // 0.0366x^2 - 0.1778x + 0.3021

		auto y = std::min(m_viewportHeight / 1000.f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;  // 0.0366x^2 - 0.1778x + 0.3021

		return {xFactor, yFactor};
	}

	float EditorCamera::rotationSpeed() { return 0.8f; }
	float EditorCamera::zoomSpeed() const
	{
		float dst = distance * 0.2f;
		dst = std::max(dst, 0.f);
		float speed = dst * dst;
		speed = std::min(speed, 100.f);
		return speed;
	}
}  // namespace MRG
