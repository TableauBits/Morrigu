//
// Created by Mathis Lamidey on 2021-10-16.
//

#include "MaterialEditorLayer.h"

#include <Morrigu.h>
#include <imgui.h>

#include <array>
#include <chrono>

class MachaLayer : public MRG::StandardLayer
{
public:
	void onAttach() override
	{
		mainCamera.aspectRatio = m_viewportSize.x / m_viewportSize.y;
		mainCamera.setPerspective(70.f, 0.001f, 1000.f);
		mainCamera.recalculateViewProjection();

		m_viewport = createFramebuffer(MRG::FramebufferSpecification{
		  .width               = 1280,
		  .height              = 720,
		  .samplingFilter      = vk::Filter::eLinear,
		  .samplingAddressMode = vk::SamplerAddressMode::eClampToEdge,
		});

		auto mesh = MRG::Utils::Meshes::torus<MRG::TexturedVertex>();
		uploadMesh(mesh);
		const auto torus = createRenderObject(mesh, application->renderer->defaultTexturedMaterial);
		torus->translate({0.f, 0.f, -5.f});
		m_sceneDrawables.emplace_back(torus);
	}

	void onUpdate(MRG::Timestep ts) override
	{
		if ((m_viewportSize.x > 0 && m_viewportSize.y > 0) &&
		    (m_viewportSize.x != m_viewport->spec.width || m_viewportSize.y != m_viewport->spec.height)) {
			m_viewport->resize(static_cast<uint32_t>(m_viewportSize.x), static_cast<uint32_t>(m_viewportSize.y));

			mainCamera.aspectRatio = m_viewportSize.x / m_viewportSize.y;
			mainCamera.recalculateViewProjection();
		}

		m_sceneDrawables[0]->rotate({1.f, 0.f, 0.f}, ts * glm::radians(15.f));
		m_sceneDrawables[0]->rotate({0.f, 1.f, 0.f}, ts * glm::radians(15.f));
		m_sceneDrawables[0]->rotate({0.f, 0.f, 1.f}, ts * glm::radians(-15.f));

		application->renderer->draw(m_sceneDrawables.begin(), m_sceneDrawables.end(), mainCamera, m_viewport);
	}

	void onEvent(MRG::Event&) override {}

	void onImGuiUpdate(MRG::Timestep ts) override
	{
		auto* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(mainViewport->Pos);
		ImGui::SetNextWindowSize(mainViewport->Size);
		ImGui::SetNextWindowViewport(mainViewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});

		bool isDockspaceOpen              = true;
		static const auto mainWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGui::Begin("Main window", &isDockspaceOpen, mainWindowFlags);
		// Immediatly reverts to default style
		ImGui::PopStyleVar(3);

		// Create dockspace
		auto& style                    = ImGui::GetStyle();
		const auto minWindowSizeBackup = style.WindowMinSize.x;
		style.WindowMinSize.x          = 70.f;
		const auto dockspaceID         = ImGui::GetID("Macha Dockspace");
		ImGui::DockSpace(dockspaceID);

		// Setup main window menu bar
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) { application->close(); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Render viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.f, 0.f});
		if (ImGui::Begin("Viewport")) {
			m_viewportSize = ImGui::GetContentRegionAvail();
			ImGui::Image(m_viewport->getImTexID(), m_viewportSize, {1, 0}, {0, 1});
		}
		ImGui::End();  // Viewport
		ImGui::PopStyleVar();

		// Debug window
		if (ImGui::Begin("Debug window")) {
			const auto color = (ts.getMilliseconds() >= 33.34f) ? ImVec4{0.8f, 0.15f, 0.15f, 1.f} : ImVec4{0.15f, 0.8f, 0.15f, 1.f};
			ImGui::TextColored(color, "Frametime: %2.5fms (%3.2f fps)", ts.getMilliseconds(), 1.f / ts);
		}
		ImGui::End();  // Debug window

		ImGui::End();  // Main window
	}

private:
	ImVec2 m_viewportSize{1280.f, 720.f};
	MRG::Ref<MRG::Framebuffer> m_viewport;

	std::vector<MRG::Ref<MRG::RenderObject<MRG::TexturedVertex>>> m_sceneDrawables{};
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application{MRG::ApplicationSpecification{.windowName            = "Macha editor",
	                                                     .rendererSpecification = {
	                                                       .applicationName      = "Macha",
	                                                       .windowWidth          = 1280,
	                                                       .windowHeight         = 720,
	                                                       .preferredPresentMode = vk::PresentModeKHR::eMailbox,
	                                                     }}}
	{}
};

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	MRG::Logger::init();
	MRG_ENGINE_TRACE("Starting engine")
	SampleApp app{};
	auto end                     = std::chrono::high_resolution_clock::now();
	const auto engineStartupTime = std::chrono::duration<float, std::milli>{end - start}.count();
	MRG_ENGINE_INFO("Engine startup time: {}ms", engineStartupTime)

	MRG_TRACE("Starting editor")
	start = std::chrono::high_resolution_clock::now();
	app.pushLayer(new MachaLayer);
	end                          = std::chrono::high_resolution_clock::now();
	const auto editorStartupTime = std::chrono::duration<float, std::milli>{end - start}.count();
	MRG_INFO("Editor startup time: {}ms", editorStartupTime)

	app.run();
}
