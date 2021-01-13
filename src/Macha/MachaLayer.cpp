#include "MachaLayer.h"

#include "Maths/Maths.h"
#include "Scene/SceneSerializer.h"
#include "Utils/FileDialogs.h"

#include <imgui.h>
///
#include <ImGuizmo.h>

#include <filesystem>

namespace MRG
{
	MachaLayer::MachaLayer() : Layer("Sandbox 2D") {}

	void MachaLayer::onAttach()
	{
		MRG_PROFILE_FUNCTION()

		m_renderTarget = Framebuffer::create({1280, 720});
		Renderer2D::setRenderTarget(m_renderTarget);
		Renderer2D::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});

		m_editorCamera = EditorCamera{30.f, 1.778f, 0.1f, 100.f};

		newScene();
	}

	void MachaLayer::onDetach() { MRG_PROFILE_FUNCTION() }

	void MachaLayer::onUpdate(Timestep ts)
	{
		MRG_PROFILE_FUNCTION()

		m_frameTime = ts;

		// handle resizing
		if (const auto spec = m_renderTarget->getSpecification();
		    m_viewportSize.x > 0.f && m_viewportSize.y > 0.f && (spec.width != m_viewportSize.x || spec.height != m_viewportSize.y)) {
			m_renderTarget->resize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
			m_editorCamera.setViewportSize(m_viewportSize.x, m_viewportSize.y);
			m_activeScene->onViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
		}

		m_editorCamera.onUpdate(ts);

		Renderer2D::resetStats();
		MRG_PROFILE_SCOPE("Render prep")
		Renderer2D::clear();

		m_activeScene->onEditorUpdate(ts, m_editorCamera);
	}

	void MachaLayer::onImGuiRender()
	{
		MRG_PROFILE_FUNCTION()

		const auto fps = 1 / m_frameTime;
		const auto tsColor = (fps < 30) ? ImVec4{1.f, 0.f, 0.f, 1.f} : ImVec4{0.f, 1.f, 0.f, 1.f};
		const auto stats = Renderer2D::getStats();

		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we
		// ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		auto& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, {0.0f, 0.0f}, dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("New scene", "Ctrl+N"))
					newScene();
				if (ImGui::MenuItem("Open scene", "Ctrl+O"))
					openScene();
				if (ImGui::MenuItem("Save scene as", "Ctrl+Shift+S"))
					saveScene();

				if (ImGui::MenuItem("Exit"))
					Application::get().close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_sceneHierarchyPanel.onImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		ImGui::Begin("Viewport");
		auto [windowX, windowY] = ImGui::GetWindowPos();
		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		Application::get().getImGuiLayer()->blockEvents(!m_viewportFocused && !m_viewportHovered);

		auto viewportSize = ImGui::GetContentRegionAvail();
		m_viewportSize = {viewportSize.x, viewportSize.y};

		ImGui::Image(m_renderTarget->getImTextureID(), viewportSize, m_renderTarget->getUVMapping()[0], m_renderTarget->getUVMapping()[1]);

		// Drawing gizmos
		auto selectedEntity = m_sceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity && m_gizmoType != -1) {
			ImGuizmo::SetDrawlist();

			const auto windowWidth = static_cast<float>(ImGui::GetWindowWidth());
			const auto windowHeight = static_cast<float>(ImGui::GetWindowHeight());
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Editor camera
			auto cameraProj = m_editorCamera.getProjection();
			auto cameraView = m_editorCamera.getViewMatrix();
			if (RenderingAPI::getAPI() == RenderingAPI::API::Vulkan)
				cameraProj[1][1] *= -1;

			// Transform
			auto& tc = selectedEntity.getComponent<TransformComponent>();
			auto transform = tc.getTransform();

			// Snapping
			bool snap = Input::isKeyPressed(Key::LeftControl);
			float snapValue = m_gizmoType == ImGuizmo::OPERATION::ROTATE ? 45.f : 0.5f;
			float snapValues[3] = {snapValue, snapValue, snapValue};

			ImGuizmo::Manipulate(glm::value_ptr(cameraView),
			                     glm::value_ptr(cameraProj),
			                     static_cast<ImGuizmo::OPERATION>(m_gizmoType),
			                     ImGuizmo::LOCAL,
			                     glm::value_ptr(transform),
			                     nullptr,
			                     snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Maths::decomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Debug");
		{
			ImGui::Text("Renderer2D stats:");
			ImGui::Text("Draw calls: %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Vertices: %d", stats.getVertexCount());
			ImGui::Text("Indices: %d", stats.getIndexCount());
			ImGui::TextColored(tsColor, "Frametime: %04.4f ms (%04.2f FPS)", m_frameTime.getMillieconds(), fps);

			auto [mouseX, mouseY] = ImGui::GetMousePos();
			glm::vec2 offsetPosition = {mouseX - windowX, mouseY - windowY};
			uint32_t id = entt::null;
			if (offsetPosition.x >= 0 && offsetPosition.y >= 0 && offsetPosition.x < viewportSize.x && offsetPosition.y < viewportSize.y) {
				id = m_activeScene->objectIDAt(static_cast<uint32_t>(offsetPosition.x), static_cast<uint32_t>(offsetPosition.y));
			}
			ImGui::Text("Hovered entity ID: %d (%f , %f)", id, offsetPosition.x, offsetPosition.y);
		}
		ImGui::End();

		ImGui::End();
	}

	void MachaLayer::onEvent(Event& event)
	{
		m_editorCamera.onEvent(event);

		EventDispatcher dispatcher{event};
		dispatcher.dispatch<KeyPressedEvent>([this](KeyPressedEvent& keyPressedEvent) { return onKeyPressed(keyPressedEvent); });
	}

	bool MachaLayer::onKeyPressed(KeyPressedEvent& event)
	{
		if (event.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl);
		bool shift = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift);
		// bool alt = Input::isKeyPressed(Key::LeftAlt) || Input::isKeyPressed(Key::RightAlt);

		switch (event.getKeyCode()) {
		// File shortcuts
		case Key::N: {
			if (control)
				newScene();
		} break;
		case Key::O: {
			if (control)
				openScene();
		} break;
		case Key::S: {
			if (control && shift)
				saveScene();
		} break;

		// Gizmos
		case Key::Q: {
			m_gizmoType = -1;
		} break;
		case Key::W: {
			m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
		} break;
		case Key::E: {
			m_gizmoType = ImGuizmo::OPERATION::ROTATE;
		} break;
		case Key::R: {
			m_gizmoType = ImGuizmo::OPERATION::SCALE;
		} break;

		default: {
			return false;
		}
		}

		return true;
	}

	void MachaLayer::newScene()
	{
		m_activeScene = createRef<Scene>();
		m_activeScene->onViewportResize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
		m_sceneHierarchyPanel.setContext(m_activeScene);
	}

	void MachaLayer::openScene()
	{
		const auto filepath = FileDialogs::openFile("Open a scene",
		                                            "Morrigu scene file",
		                                            {"*.morrigu"},
		                                            fmt::format("{}/runtime/scenes", std::filesystem::current_path().string()).c_str());
		if (!filepath)
			return;

		newScene();
		SceneSerializer serializer{m_activeScene};
		serializer.deserialize(filepath.value());
	}

	void MachaLayer::saveScene()
	{
		const auto filepath = FileDialogs::saveFile("Save a scene as",
		                                            "Morrigu scene file",
		                                            {"*.morrigu"},
		                                            fmt::format("{}/runtime/scenes", std::filesystem::current_path().string()).c_str());
		if (!filepath)
			return;

		SceneSerializer serializer{m_activeScene};
		serializer.serialize(filepath.value());
	}
}  // namespace MRG
