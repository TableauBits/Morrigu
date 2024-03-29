//
// Created by Mathis Lamidey on 2021-10-16.
//

#include "MaterialEditorLayer.h"

#include "Panels/AssetPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Panels/Viewport.h"
#include "Scene.h"

#include <ImGuizmo.h>
#include <imgui.h>

#include <chrono>
#include <map>

class MachaLayer : public MRG::StandardLayer
{
public:
	void onAttach() override
	{
		m_viewport = MRG::createRef<Viewport>(application, ImVec2{1280.f, 720.f});

		m_hierarchyPanel                              = MRG::createRef<HierarchyPanel>();
		m_hierarchyPanel->callbacks.entityCreation    = [this]() { auto entity = m_activeScene.createEntity(); };
		m_hierarchyPanel->callbacks.entityDestruction = [this](const entt::entity entity) { m_activeScene.destroyEntity(entity); };
		m_hierarchyPanel->callbacks.entitySelected    = [this](const entt::entity entity) { m_activeScene.selectedEntity = entity; };

		m_assetPanel = MRG::createRef<AssetPanel>(*application->renderer);

		// auto material = createMaterial<MRG::TexturedVertex>(createShader("TestShader.vert.spv", "TestShader.frag.spv"));
		auto material = application->renderer->defaultTexturedMaterial;

		auto torus = m_activeScene.createEntity();
		torus.setName("Torus");
		auto torusMesh = MRG::Utils::Meshes::torus<MRG::TexturedVertex>();
		uploadMesh(torusMesh);
		auto& torusMRC = torus.addComponent<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(createMeshRenderer(torusMesh, material));
		auto& torusTC  = torus.getComponent<MRG::Components::Transform>();
		torusTC.translation = {1.5f, 0.f, 0.f};
		torusMRC.updateTransform(torusTC.getTransform());

		auto cylinder = m_activeScene.createEntity();
		cylinder.setName("Cylinder");
		auto cylinderMesh = MRG::Utils::Meshes::cylinder<MRG::TexturedVertex>();
		uploadMesh(cylinderMesh);
		auto& cylinderMRC =
		  cylinder.addComponent<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(createMeshRenderer(cylinderMesh, material));
		auto& cylinderTC       = cylinder.getComponent<MRG::Components::Transform>();
		cylinderTC.translation = {-1.5f, 0.f, 0.f};
		cylinderMRC.updateTransform(cylinderTC.getTransform());
	}

	void onUpdate(MRG::Timestep ts) override
	{
		// Update viewport
		m_viewport->onUpdate(*m_activeScene.registry, ts);
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
			if (ImGui::BeginMenu("Debug")) {
				ImGui::MenuItem("ImGui demo window", nullptr, &showDemoWindow);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Render ImGui demo window if requested
		if (showDemoWindow) { ImGui::ShowDemoWindow(); }

		// Render viewport
		m_viewport->onImGuiUpdate(m_activeScene.selectedEntity, *m_activeScene.registry);

		// Render hierarchy panel
		m_hierarchyPanel->onImGuiUpdate(*m_activeScene.registry, m_activeScene.selectedEntity);

		// Render properties panel
		PropertiesPanel::onImGuiUpdate(m_activeScene.selectedEntity, *m_activeScene.registry, m_activeScene.assets, *application->renderer);

		// Render asset panel
		m_assetPanel->onImGuiRender();

		// Debug window
		if (ImGui::Begin("Debug window")) {
			const auto color = (ts.getMilliseconds() >= 33.34f) ? ImVec4{0.8f, 0.15f, 0.15f, 1.f} : ImVec4{0.15f, 0.8f, 0.15f, 1.f};
			ImGui::TextColored(
			  color, "Frametime: %2.5fms (%3.2f fps)", static_cast<double>(ts.getMilliseconds()), static_cast<double>(1.f / ts));

			const auto& entity = m_activeScene.selectedEntity;
			if (entity != entt::null) {
				ImGui::Text("Selected entity ID: %d (%s)",
				            static_cast<uint32_t>(entity),
				            m_activeScene.registry->get<MRG::Components::Tag>(entity).tag.c_str());
			} else {
				ImGui::Text("No entity selected");
			}
		}
		ImGui::End();  // Debug window

		ImGui::End();  // Main window
	}

private:
	bool showDemoWindow{false};

	MRG::Ref<Viewport> m_viewport;
	MRG::Ref<HierarchyPanel> m_hierarchyPanel{};
	MRG::Ref<AssetPanel> m_assetPanel{};

	Scene m_activeScene{};
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application{MRG::ApplicationSpecification{.windowName            = "Macha editor",
	                                                     .maximized             = true,
	                                                     .rendererSpecification = {
	                                                       .applicationName      = "Macha",
	                                                       .preferredPresentMode = vk::PresentModeKHR::eMailbox,
	                                                     }}}
	{}
};

int main()
{
	auto start = std::chrono::high_resolution_clock::now();
	MRG::Logger::init();
	MRG_ENGINE_INFO("Starting engine")
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
