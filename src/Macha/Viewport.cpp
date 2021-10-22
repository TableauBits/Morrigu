//
// Created by Mathis Lamidey on 2021-10-17.
//

#include "Viewport.h"

Viewport::Viewport(MRG::Application* context, ImVec2 initialSize) : m_context{context}, m_size{initialSize}
{
	camera.aspectRatio = m_size.x / m_size.y;
	camera.setPerspective(glm::radians(90.f), 0.001f, 1000.f);
	camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
	camera.recalculateViewProjection();

	m_framebuffer = context->renderer->createFrameBuffer(MRG::FramebufferSpecification{
	  .width               = static_cast<uint32_t>(m_size.x),
	  .height              = static_cast<uint32_t>(m_size.y),
	  .samplingFilter      = vk::Filter::eLinear,
	  .samplingAddressMode = vk::SamplerAddressMode::eClampToEdge,
	});

	m_texID = m_framebuffer->getImTexID();
}

void Viewport::onUpdate(const std::vector<MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>>>& drawables, MRG::Timestep ts)
{
	if ((m_size.x > 0 && m_size.y > 0) && (m_size.x != m_framebuffer->spec.width || m_size.y != m_framebuffer->spec.height)) {
		m_framebuffer->resize(static_cast<uint32_t>(m_size.x), static_cast<uint32_t>(m_size.y));

		camera.aspectRatio = m_size.x / m_size.y;
		camera.recalculateViewProjection();
	}

	// Input
	if (m_isFocused && !(ImGuizmo::IsOver() || ImGuizmo::IsUsing())) {
		// Mouse input
		if (ImGui::IsKeyDown(MRG::Key::LeftAlt)) {
			const auto delta = (m_currentMousePos - m_lastMousePos) * 0.003f;

			if (ImGui::IsMouseDown(MRG::Mouse::ButtonMiddle)) { mousePan(delta); }
			if (ImGui::IsMouseDown(MRG::Mouse::ButtonLeft)) { mouseRotate(delta); }
			if (ImGui::IsMouseDown(MRG::Mouse::ButtonRight)) { mouseZoom(delta.y * 5.f); }
		}

		// Keyboard input
		if (ImGui::IsKeyDown(MRG::Key::W)) {
			m_focalPoint += camera.getForwardVector() * ts.getSeconds() * moveSpeed;
			camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
			camera.recalculateViewProjection();
		}
		if (ImGui::IsKeyDown(MRG::Key::S)) {
			m_focalPoint -= camera.getForwardVector() * ts.getSeconds() * moveSpeed;
			camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
			camera.recalculateViewProjection();
		}
		if (ImGui::IsKeyDown(MRG::Key::A)) {
			m_focalPoint += camera.getRightVector() * ts.getSeconds() * moveSpeed;
			camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
			camera.recalculateViewProjection();
		}
		if (ImGui::IsKeyDown(MRG::Key::D)) {
			m_focalPoint -= camera.getRightVector() * ts.getSeconds() * moveSpeed;
			camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
			camera.recalculateViewProjection();
		}
	}

	m_context->renderer->draw(drawables.begin(), drawables.end(), camera, m_framebuffer);
}

void Viewport::onImGuiUpdate(MRG::Timestep)
{
	ImGuizmo::BeginFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});
	if (ImGui::Begin("Viewport")) {
		m_position  = ImGui::GetWindowPos();
		m_size      = ImGui::GetContentRegionAvail();
		m_isFocused = ImGui::IsWindowFocused();
		m_isHovered = ImGui::IsWindowHovered();

		const auto temp   = ImGui::GetMousePos();
		m_lastMousePos    = m_currentMousePos;
		m_currentMousePos = {temp.x, temp.y};

		float snap = (guizmoType == ImGuizmo::OPERATION::ROTATE) ? 45.f : 0.5f;
		std::array<float, 3> snapValues{snap, snap, snap};

		if (selectedEntity != nullptr) {
			const auto windowPos = ImGui::GetWindowPos();
			ImGuizmo::SetRect(windowPos.x, windowPos.y, m_size.x, m_size.y);
			auto projection = camera.getProjection();
			projection[0][0] *= -1;
			projection[1][1] *= -1;
			ImGuizmo::Manipulate(glm::value_ptr(camera.getView()),
			                     glm::value_ptr(projection),
			                     guizmoType,
			                     ImGuizmo::MODE::LOCAL,
			                     glm::value_ptr(*selectedEntity->modelMatrix),
			                     nullptr,
			                     ImGui::IsKeyDown(MRG::Key::LeftControl) ? snapValues.data() : nullptr);
			selectedEntity->uploadPosition();
		}

		ImGui::Image(m_texID, m_size, {1, 0}, {0, 1});
	}
	ImGui::End();  // Viewport
	ImGui::PopStyleVar();
}

bool Viewport::onEvent(MRG::Event& event)
{
	if (!m_isFocused && !m_isHovered) return false;
	bool handled = false;
	MRG::EventDispatcher dispatcher{event};

	handled |= dispatcher.dispatch<MRG::KeyPressedEvent>([this](MRG::KeyPressedEvent& kPressEvent) { return onKeyPressed(kPressEvent); });
	handled |= dispatcher.dispatch<MRG::MouseButtonPressedEvent>(
	  [this](MRG::MouseButtonPressedEvent& mPressEvent) { return onMousePressed(mPressEvent); });
	handled |=
	  dispatcher.dispatch<MRG::MouseScrolledEvent>([this](MRG::MouseScrolledEvent& scrollEvent) { return onMouseScrolled(scrollEvent); });

	return handled;
}

bool Viewport::onKeyPressed(MRG::KeyPressedEvent& keyPress)
{
	if (keyPress.getRepeatCount() > 0) { return false; }

	switch (keyPress.getKeyCode()) {
	case MRG::Key::D1:
		if (!ImGuizmo::IsUsing()) { guizmoType = ImGuizmo::OPERATION::TRANSLATE; }
		break;

	case MRG::Key::D2:
		if (!ImGuizmo::IsUsing()) { guizmoType = ImGuizmo::OPERATION::ROTATE; }
		break;

	case MRG::Key::D3:
		if (!ImGuizmo::IsUsing()) { guizmoType = ImGuizmo::OPERATION::SCALE; }
		break;

	default:
		break;
	}

	return false;
}

bool Viewport::onMousePressed(MRG::MouseButtonPressedEvent&) { return false; }

bool Viewport::onMouseScrolled(MRG::MouseScrolledEvent& scrollEvent)
{
	if (!m_isFocused && !m_isHovered) { return false; }
	float delta = scrollEvent.getY() * 0.4f;
	mouseZoom(delta);

	return false;
}

void Viewport::mousePan(const glm::vec2& delta)
{
	auto x       = std::min(m_size.x / 1000.f, 2.4f);
	float xSpeed = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;  // 0.0366x^2 - 0.1778x + 0.3021

	auto y       = std::min(m_size.y / 1000.f, 2.4f);
	float ySpeed = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;  // 0.0366x^2 - 0.1778x + 0.3021

	m_focalPoint += camera.getRightVector() * delta.x * xSpeed * m_distance;
	m_focalPoint += camera.getUpVector() * delta.y * ySpeed * m_distance;

	camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
	camera.recalculateViewProjection();
}

void Viewport::mouseRotate(const glm::vec2& delta)
{
	camera.yaw += -1.f * delta.x * 0.8f;
	camera.pitch += delta.y * 0.8f;

	camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
	camera.recalculateViewProjection();
}

void Viewport::mouseZoom(float delta)
{
	float dst   = m_distance * 0.2f;
	dst         = std::max(dst, 0.f);
	float speed = dst * dst;
	speed       = std::min(speed, 100.f);

	m_distance -= delta * speed;
	if (m_distance < 1.f) { m_distance = 1.f; }

	camera.position = m_focalPoint - camera.getForwardVector() * m_distance;
	camera.recalculateViewProjection();
}
