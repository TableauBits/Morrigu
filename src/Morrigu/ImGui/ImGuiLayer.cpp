#include "ImGuiLayer.h"

#include "Core/Application.h"
#include "Debug/Instrumentor.h"
#include "Renderer/RenderingAPI.h"

#include <ImGui/bindings/imgui_impl_glfw.h>
#include <ImGui/bindings/imgui_impl_opengl3.h>
#include <ImGui/bindings/imgui_impl_vulkan.h>
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

		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 460 core");
		} break;

		case RenderingAPI::API::Vulkan: {
			// nothing for now
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
		} break;
		}
	}

	void ImGuiLayer::onDetach()
	{
		MRG_PROFILE_FUNCTION();

		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		} break;

		case RenderingAPI::API::Vulkan: {
			// nothing for now
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
		} break;
		}

		ImGui::DestroyContext();
	}

	void ImGuiLayer::begin()
	{
		MRG_PROFILE_FUNCTION();

		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		} break;

		case RenderingAPI::API::Vulkan: {
			// nothing for now
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
		} break;
		}
	}

	void ImGuiLayer::end()
	{
		MRG_PROFILE_FUNCTION();

		auto& io = ImGui::GetIO();
		auto& app = Application::get();
		io.DisplaySize = ImVec2{static_cast<float>(app.getWindow().getWidth()), static_cast<float>(app.getWindow().getHeight())};

		switch (RenderingAPI::getAPI()) {
		case RenderingAPI::API::OpenGL: {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				const auto contextBkp = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(contextBkp);
			}
		} break;

		case RenderingAPI::API::Vulkan: {
			// nothing for now
		} break;

		case RenderingAPI::API::None:
		default: {
			MRG_CORE_ASSERT(false, fmt::format("UNSUPPORTED RENDERER API OPTION ! ({})", RenderingAPI::getAPI()));
		} break;
		}
	}
}  // namespace MRG