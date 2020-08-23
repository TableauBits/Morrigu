#include "ImGuiLayer.h"

#include "Core/Application.h"
#include "Debug/Instrumentor.h"

#include <ImGui/bindings/imgui_impl_glfw.h>
#include <ImGui/bindings/imgui_impl_opengl3.h>
#include <imgui.h>

namespace MRG
{
	ImGuiLayer::ImGuiLayer() : Layer{"ImGui Layer"} {}

	void ImGuiLayer::onAttach()
	{
		MRG_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.f;
			style.Colors[ImGuiCol_WindowBg].w = 1.f;
		}

		const auto window = Application::get().getWindow().getGLFWWindow();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}

	void ImGuiLayer::onDetach()
	{
		MRG_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::begin()
	{
		MRG_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::end()
	{
		MRG_PROFILE_FUNCTION();

		auto& io = ImGui::GetIO();
		auto& app = Application::get();
		io.DisplaySize = ImVec2{static_cast<float>(app.getWindow().getWidth()), static_cast<float>(app.getWindow().getHeight())};

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			const auto contextBkp = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(contextBkp);
		}
	}
}  // namespace MRG