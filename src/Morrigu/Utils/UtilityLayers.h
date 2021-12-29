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
			application->renderer->uploadMesh(mesh);
		}

		[[nodiscard]] Ref<Framebuffer> createFramebuffer(const FramebufferSpecification& spec);
		[[nodiscard]] Ref<Texture> createTexture(const std::string& filePath);
		[[nodiscard]] Ref<Shader> createShader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

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
