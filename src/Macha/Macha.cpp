//
// Created by Mathis Lamidey on 2021-10-16.
//

#include "MaterialEditorLayer.h"

#include "Viewport.h"

#include <ImGuizmo.h>
#include <Morrigu.h>
#include <imgui.h>

#include <array>
#include <chrono>

class MachaLayer : public MRG::StandardLayer
{
public:
	void onAttach() override
	{
		m_viewport = MRG::createRef<Viewport>(application, ImVec2{1280.f, 720.f});

		auto mesh = MRG::Utils::Meshes::torus<MRG::TexturedVertex>();
		uploadMesh(mesh);
		const auto torus = createRenderObject(mesh, application->renderer->defaultTexturedMaterial);
		m_sceneDrawables.emplace_back(torus);

		m_viewport->selectedEntity = torus;

		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
	}

	void onUpdate(MRG::Timestep ts) override
	{
		// Update viewport
		m_viewport->onUpdate(m_sceneDrawables, ts);
	}

	void onEvent(MRG::Event& event) override { m_viewport->onEvent(event); }

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

		style.WindowMinSize.x = minWindowSizeBackup;

		// Setup main window menu bar
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) { application->close(); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Render viewport
		m_viewport->onImGuiUpdate(ts);

		// Debug window
		if (ImGui::Begin("Debug window")) {
			const auto color = (ts.getMilliseconds() >= 33.34f) ? ImVec4{0.8f, 0.15f, 0.15f, 1.f} : ImVec4{0.15f, 0.8f, 0.15f, 1.f};
			ImGui::TextColored(color, "Frametime: %2.5fms (%3.2f fps)", static_cast<double>(ts.getMilliseconds()), static_cast<double>(1.f / ts));
		}
		ImGui::End();  // Debug window

		ImGui::End();  // Main window
	}

private:
	MRG::Ref<Viewport> m_viewport;
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
