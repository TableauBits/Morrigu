//
// Created by Mathis Lamidey on 2021-10-17.
//

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <Morrigu.h>
#include <imgui.h>

class Viewport
{
public:
	Viewport(MRG::Application* context, ImVec2 initialSize);

	void onUpdate(const std::vector<MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>>>& drawables, MRG::Timestep ts);
	void onImGuiUpdate(MRG::Timestep ts);
	bool onEvent(MRG::Event& event);

	MRG::StandardCamera camera{};

private:
	bool onKeyPressed(MRG::KeyPressedEvent& keyPress);
	bool onMousePressed(MRG::MouseButtonPressedEvent& mousePress);
	bool onMouseScrolled(MRG::MouseScrolledEvent& mouseScroll);

	void mousePan(const glm::vec2& delta);
	void mouseRotate(const glm::vec2& delta);
	void mouseZoom(float delta);

	MRG::Application* m_context;
	MRG::Ref<MRG::Framebuffer> m_framebuffer;
	ImTextureID m_texID;

	// Viewport Status
	ImVec2 m_size{};
	ImVec2 m_position{};
	bool m_isFocused{false};
	bool m_isHovered{false};

	// Camera settings
	glm::vec2 m_lastMousePos{};
	glm::vec2 m_currentMousePos{};
	glm::vec3 m_focalPoint{};
	float m_distance{5.f};
};

#endif
