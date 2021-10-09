//
// Created by Mathis Lamidey on 2021-07-23.
//

#ifndef MORRIGU_SAMPLELAYERS_H
#define MORRIGU_SAMPLELAYERS_H

#include "Core/Application.h"
#include "Core/Layer.h"
#include "Rendering/UI/Text.h"

namespace MRG
{
	class StandardLayer : public Layer
	{
	public:
		StandardLayer();

		Ref<Camera> getMainCamera() override;

		template<Vertex VertexType>
		void uploadMesh(Ref<Mesh<VertexType>>& mesh)
		{
			application->renderer->uploadMesh(mesh);
		}

		[[nodiscard]] Ref<UI::Font> createFont(const std::string& filePath) { return application->renderer->createFont(filePath); }

		[[nodiscard]] Ref<UI::Text> createText(const std::string& content, const Ref<UI::Font>& font, const glm::vec2& position)
		{
			return createRef<UI::Text>(content, font, position, application->renderer.get());
		}

		[[nodiscard]] Ref<Shader> createShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
		{
			return application->renderer->createShader(vertexShaderFile.c_str(), fragmentShaderFile.c_str());
		}

		template<Vertex VertexType>
		[[nodiscard]] Ref<Material<VertexType>> createMaterial(const Ref<Shader>& shader, const MaterialConfiguration& config = {})
		{
			return application->renderer->createMaterial<VertexType>(shader, config);
		}

		template<Vertex VertexType>
		[[nodiscard]] Ref<RenderObject<VertexType>> createRenderObject(const Ref<Mesh<VertexType>>& mesh,
		                                                               const Ref<Material<VertexType>>& material)
		{
			return application->renderer->createRenderObject(mesh, material);
		}

		[[nodiscard]] Ref<Framebuffer> createFramebuffer(const FramebufferSpecification& spec)
		{
			return application->renderer->createFrameBuffer(spec);
		}

		Ref<StandardCamera> mainCamera{};
	};

	// This layer is an orthographic pixel perfect layer
	class UILayer : public StandardLayer
	{
	public:
		UILayer();

		Ref<Camera> getMainCamera() override;

		void onAttach() override;
		void onEvent(MRG::Event& event) override;

		Ref<PixelPerfectCamera> mainCamera{};
	};
}  // namespace MRG

#endif  // MORRIGU_SAMPLELAYERS_H
