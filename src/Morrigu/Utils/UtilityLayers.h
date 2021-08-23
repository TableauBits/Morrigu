//
// Created by Mathis Lamidey on 2021-07-23.
//

#ifndef MORRIGU_SAMPLELAYERS_H
#define MORRIGU_SAMPLELAYERS_H

#include "Core/Application.h"
#include "Core/Layer.h"

namespace MRG
{
	class StandardLayer : public Layer
	{
	public:
		template<Vertex VertexType>
		void uploadMesh(Ref<Mesh<VertexType>>& mesh)
		{
			application->renderer.uploadMesh(mesh);
		}

		[[nodiscard]] Ref<UI::Font> createFont(const std::string& filePath) { return application->renderer.createFont(filePath); }

		[[nodiscard]] Ref<UI::Text> createText(const std::string& content, const Ref<UI::Font>& font)
		{
			return createRef<UI::Text>(content, font, &application->renderer);
		}

		[[nodiscard]] Ref<Shader> createShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		{
			return application->renderer.createShader(vertexShaderFile.c_str(), fragmentShaderFile.c_str());
		}

		template<Vertex VertexType>
		[[nodiscard]] Ref<Material<VertexType>> createMaterial(const Ref<Shader> shader, const MaterialConfiguration& config = {})
		{
			return application->renderer.createMaterial<VertexType>(shader, config);
		}

		template<Vertex VertexType>
		[[nodiscard]] Ref<RenderObject<VertexType>> createRenderObject(const Ref<Mesh<VertexType>>& mesh,
		                                                               const Ref<Material<VertexType>>& material)
		{
			return application->renderer.createRenderObject(mesh, material);
		}
	};

	class UILayer : public StandardLayer
	{
	public:
		void onAttach() override
		{
			mainCamera.aspectRatio =
			  static_cast<float>(application->renderer.spec.windowWidth) / static_cast<float>(application->renderer.spec.windowHeight);
			mainCamera.setOrthgraphic(2.f, -1.f, 1.f);
			mainCamera.recalculateViewProjection();
		}

		void onEvent(MRG::Event& event) override
		{
			MRG::EventDispatcher dispatcher{event};
			dispatcher.dispatch<MRG::WindowResizeEvent>([this](MRG::WindowResizeEvent& event) { return mainCamera.onResize(event); });
		}
	};
}  // namespace MRG

#endif  // MORRIGU_SAMPLELAYERS_H
