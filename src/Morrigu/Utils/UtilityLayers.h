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
		template<Vertex VertexType>
		void uploadMesh(Ref<Mesh<VertexType>>& mesh)
		{
			application->renderer->uploadMesh(mesh);
		}

		[[nodiscard]] Ref<UI::Font> createFont(const std::string& filePath);
		[[nodiscard]] Ref<UI::Text> createText(const std::string& content, const Ref<UI::Font>& font, const glm::vec2& position);
		[[nodiscard]] Ref<Shader> createShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
		[[nodiscard]] Ref<Entity> createEntity();
		[[nodiscard]] Ref<Framebuffer> createFramebuffer(const FramebufferSpecification& spec);

		template<Vertex VertexType>
		[[nodiscard]] Ref<Material<VertexType>> createMaterial(const Ref<Shader>& shader, const MaterialConfiguration& config = {})
		{
			return application->renderer->createMaterial<VertexType>(shader, config);
		}
		template<Vertex VertexType>
		[[nodiscard]] Components::MeshRenderer<VertexType>::VulkanObjects createMeshRenderer(const Ref<Mesh<VertexType>>& meshRef,
		                                                                                     const Ref<Material<VertexType>>& materialRef)
		{
			return application->renderer->createMeshRenderer<VertexType>(meshRef, materialRef);
		}

		Ref<entt::registry> registry{createRef<entt::registry>()};
	};

	// This layer is an orthographic pixel perfect layer
	class UILayer : public StandardLayer
	{
	public:
		UILayer();

		void onAttach() override;
		void onEvent(MRG::Event& event) override;

		PixelPerfectCamera mainCamera{};
	};
}  // namespace MRG

#endif  // MORRIGU_SAMPLELAYERS_H
