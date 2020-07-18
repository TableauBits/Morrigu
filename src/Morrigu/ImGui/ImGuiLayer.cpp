#include "ImGuiLayer.h"

#include "Core/Application.h"

#include "GLFW/glfw3.h"
#include "ImGui/bindings/imgui_impl_opengl3.h"
#include "glad/glad.h"

namespace MRG
{
	ImGuiLayer::ImGuiLayer() : Layer{"ImGui Layer"}, m_cumulativeTime{0.0f} {}

	void ImGuiLayer::onAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 460 core");
	}

	void ImGuiLayer::onDetach() {}

	void ImGuiLayer::onUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2{static_cast<float>(app.getWindow().getWidth()), static_cast<float>(app.getWindow().getHeight())};

		float time = static_cast<float>(glfwGetTime());
		io.DeltaTime = (m_cumulativeTime > 0) ? time - m_cumulativeTime : (1.f / 60.f);
		m_cumulativeTime = time;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::onEvent(Event& event)
	{
		EventDispatcher dispatcher{event};
		dispatcher.dispatch<MouseButtonPressedEvent>(
		  [this](MouseButtonPressedEvent& event) -> bool { return onMouseButtonPressedEvent(event); });
		dispatcher.dispatch<MouseButtonReleasedEvent>(
		  [this](MouseButtonReleasedEvent& event) -> bool { return onMouseButtonReleasedEvent(event); });
		dispatcher.dispatch<MouseMovedEvent>([this](MouseMovedEvent& event) -> bool { return onMouseMovedEvent(event); });
		dispatcher.dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) -> bool { return onMouseScrolledEvent(event); });
		dispatcher.dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) -> bool { return onKeyPressedEvent(event); });
		dispatcher.dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& event) -> bool { return onKeyReleasedEvent(event); });
		dispatcher.dispatch<KeyTypedEvent>([this](KeyTypedEvent& event) -> bool { return onKeyTypedEvent(event); });
		dispatcher.dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) -> bool { return onWindowResizeEvent(event); });
	}

	bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.getMouseButton()] = true;

		return false;
	}

	bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[event.getMouseButton()] = false;

		return false;
	}

	bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2{event.getX(), event.getY()};

		return false;
	}

	bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += event.getX();
		io.MouseWheel += event.getY();

		return false;
	}

	bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.getKeyCode()] = true;
		updateSpecialKeys(io);

		return false;
	}

	bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[event.getKeyCode()] = false;
		updateSpecialKeys(io);

		return false;
	}

	bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(event.getKeyCode());

		return false;
	}

	bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(event.getWidth(), event.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.f, 1.f);
		glViewport(0, 0, event.getWidth(), event.getHeight());

		return false;
	}

	void ImGuiLayer::updateSpecialKeys(ImGuiIO& io)
	{
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	}
}  // namespace MRG