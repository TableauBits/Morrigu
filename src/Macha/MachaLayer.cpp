#include "MachaLayer.h"

namespace MRG
{
	MachaLayer::MachaLayer() : Layer("Sandbox 2D"), m_camera(1280.f / 720.f) {}

	void MachaLayer::onAttach()
	{
		MRG_PROFILE_FUNCTION();

		m_checkerboard = Texture2D::create("resources/textures/Checkerboard.png");
		m_camera.movementSpeed = 2.f;

		m_renderTarget = Framebuffer::create({1280, 720});
		Renderer2D::setRenderTarget(m_renderTarget);
		Renderer2D::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});

		m_activeScene = createRef<Scene>();

		m_squareEntity = m_activeScene->createEntity();
		m_activeScene->reg().emplace<TransformComponent>(m_squareEntity);
		m_activeScene->reg().emplace<SpriteRendererComponent>(m_squareEntity, glm::vec4{0.f, 1.f, 0.f, 1.f});
	}

	void MachaLayer::onDetach() { MRG_PROFILE_FUNCTION(); }

	void MachaLayer::onUpdate(Timestep ts)
	{
		MRG_PROFILE_FUNCTION();

		m_frameTime = ts;

		// handle resizing
		if (const auto spec = m_renderTarget->getSpecification();
		    m_viewportSize.x > 0.f && m_viewportSize.y > 0.f && (spec.width != m_viewportSize.x || spec.height != m_viewportSize.y)) {
			m_renderTarget->resize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));
			m_camera.onResize(m_viewportSize.x, m_viewportSize.y);
		}

		if (m_viewportFocused)
			m_camera.onUpdate(ts);

		Renderer2D::resetStats();
		MRG_PROFILE_SCOPE("Render prep");
		Renderer2D::clear();

		Renderer2D::beginScene(m_camera.getCamera());
		m_activeScene->onUpdate(ts);
		Renderer2D::endScene();
	}

	void MachaLayer::onImGuiRender()
	{
		MRG_PROFILE_FUNCTION();

		const auto fps = 1 / m_frameTime;
		const auto color = (fps < 30) ? ImVec4{1.f, 0.f, 0.f, 1.f} : ImVec4{0.f, 1.f, 0.f, 1.f};
		const auto stats = Renderer2D::getStats();

		static bool dockspaceOpen = true;
		bool opt_fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

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

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, {0.0f, 0.0f}, dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				// ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit"))
					Application::get().close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Begin("Debug");
		{
			ImGui::Text("Renderer2D stats:");
			ImGui::Text("Draw calls: %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Vertices: %d", stats.getVertexCount());
			ImGui::Text("Indices: %d", stats.getIndexCount());
			ImGui::Separator();
			ImGui::TextColored(color, "Frametime: %04.4f ms (%04.2f FPS)", m_frameTime.getMillieconds(), fps);

			auto& squareColor = m_activeScene->reg().get<SpriteRendererComponent>(m_squareEntity).color;
			ImGui::ColorEdit4("Shader color", glm::value_ptr(squareColor));
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
		ImGui::Begin("Viewport");
		{
			m_viewportFocused = ImGui::IsWindowFocused();
			m_viewportHovered = ImGui::IsWindowHovered();
			Application::get().getImGuiLayer()->blockEvents(!m_viewportFocused || !m_viewportHovered);

			auto viewportSize = ImGui::GetContentRegionAvail();
			m_viewportSize = {viewportSize.x, viewportSize.y};

			ImGui::Image(
			  m_renderTarget->getImTextureID(), viewportSize, m_renderTarget->getUVMapping()[0], m_renderTarget->getUVMapping()[1]);
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void MachaLayer::onEvent(Event& event) { m_camera.onEvent(event); }
}  // namespace MRG
