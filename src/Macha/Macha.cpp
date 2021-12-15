//
// Created by Mathis Lamidey on 2021-10-16.
//

#include "MaterialEditorLayer.h"

#include "Components/EntitySettings.h"
#include "HierarchyPanel.h"
#include "PropertiesPanel.h"
#include "Viewport.h"

#include <ImGuizmo.h>
#include <imgui.h>

#include <array>
#include <chrono>
#include <map>

class MachaLayer : public MRG::StandardLayer
{
public:
	void onAttach() override
	{
		m_viewport = MRG::createRef<Viewport>(application, ImVec2{1280.f, 720.f});

		m_hierarchyPanel                           = MRG::createRef<HierarchyPanel>();
		m_hierarchyPanel->callbacks.entityCreation = [this]() {
			auto entity = createEntity();
			m_entities.insert(std::make_pair(entity.getHandle(), std::move(entity)));
		};
		m_hierarchyPanel->callbacks.entityDestruction = [this](const entt::entity entityHandle) { m_entities.erase(entityHandle); };
		m_hierarchyPanel->callbacks.entitySelected    = [this](const entt::entity entity) { m_viewport->selectedEntity = entity; };

		auto material = createMaterial<MRG::TexturedVertex>(createShader("TestShader.vert.spv", "TestShader.frag.spv"));

		auto torus = createEntity();
		torus.setName("Torus");
		auto torusMesh = MRG::Utils::Meshes::torus<MRG::TexturedVertex>();
		uploadMesh(torusMesh);
		auto& torusMRC = torus.addComponent<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(createMeshRenderer(torusMesh, material));
		auto& torusTC  = torus.getComponent<MRG::Components::Transform>();
		torusTC.translation = {1.5f, 0.f, 0.f};
		torusMRC.updateTransform(torusTC.getTransform());
		m_entities.insert(std::make_pair(torus.getHandle(), std::move(torus)));

		auto cylinder = createEntity();
		cylinder.setName("Cylinder");
		auto cylinderMesh = MRG::Utils::Meshes::cylinder<MRG::TexturedVertex>();
		uploadMesh(cylinderMesh);
		auto& cylinderMRC =
		  cylinder.addComponent<MRG::Components::MeshRenderer<MRG::TexturedVertex>>(createMeshRenderer(cylinderMesh, material));
		auto& cylinderTC       = cylinder.getComponent<MRG::Components::Transform>();
		cylinderTC.translation = {-1.5f, 0.f, 0.f};
		cylinderMRC.updateTransform(cylinderTC.getTransform());
		m_entities.insert(std::make_pair(cylinder.getHandle(), std::move(cylinder)));
	}

	void onUpdate(MRG::Timestep ts) override
	{
		// Update viewport
		m_viewport->onUpdate(*registry, ts);
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
		m_viewport->onImGuiUpdate(*registry);

		// Render hierarchy panel
		m_hierarchyPanel->onImGuiUpdate(m_entities);

		// Render properties panel
		PropertiesPanel::onImGuiUpdate(m_hierarchyPanel->selectedEntity, *registry);

		// Debug window
		if (ImGui::Begin("Debug window")) {
			const auto color = (ts.getMilliseconds() >= 33.34f) ? ImVec4{0.8f, 0.15f, 0.15f, 1.f} : ImVec4{0.15f, 0.8f, 0.15f, 1.f};
			ImGui::TextColored(
			  color, "Frametime: %2.5fms (%3.2f fps)", static_cast<double>(ts.getMilliseconds()), static_cast<double>(1.f / ts));

			const auto& entity = m_hierarchyPanel->selectedEntity;
			if (entity != entt::null) {
				ImGui::Text(
				  "Selected entity ID: %d (%s)", static_cast<uint32_t>(entity), registry->get<MRG::Components::Tag>(entity).tag.c_str());
			} else {
				ImGui::Text("No entity selected");
			}
		}
		ImGui::End();  // Debug window

		ImGui::End();  // Main window
	}

private:
	[[nodiscard]] MRG::Entity createEntity()
	{
		auto entity = MRG::Entity{registry};
		entity.addComponent<Components::EntitySettings>();

		return entity;
	}

	MRG::Ref<Viewport> m_viewport;
	MRG::Ref<HierarchyPanel> m_hierarchyPanel{};
	std::unordered_map<entt::entity, MRG::Entity> m_entities{};
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
