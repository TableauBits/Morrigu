//
// Created by Mathis Lamidey on 2021-04-03.
//

#include <Morrigu.h>
#include <tiny_obj_loader.h>

/// Filepath is already pointing to the meshes folder, so you can omit the path to that folder yourself
MRG::Ref<MRG::Mesh<MRG::ColoredVertex>> loadFromFile(const char* filePath)
{
	auto newMesh = MRG::createRef<MRG::Mesh<MRG::ColoredVertex>>();

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string errors;

	const auto result = tinyobj::LoadObj(&attrib, &shapes, &materials, &errors, (MRG::Folders::Rendering::meshesFolder + filePath).c_str());
	if (!result) { MRG_ENGINE_WARN("Errors encountered when loading mesh:\n\"{}\"", errors) }

	for (const auto& shape : shapes) {
		std::size_t indexOffset = 0;
		for ([[maybe_unused]] const auto& face : shape.mesh.num_face_vertices) {
			static const int shapeVertexCount = 3;  // Only triangles for now
			for (int vertexIndex = 0; vertexIndex < shapeVertexCount; ++vertexIndex) {
				const auto idx = shape.mesh.indices[indexOffset + vertexIndex];

				// position
				const auto posX = attrib.vertices[3 * idx.vertex_index + 0];
				const auto posY = attrib.vertices[3 * idx.vertex_index + 1];
				const auto posZ = attrib.vertices[3 * idx.vertex_index + 2];

				// normals
				const auto normX = attrib.normals[3 * idx.normal_index + 0];
				const auto normY = attrib.normals[3 * idx.normal_index + 1];
				const auto normZ = attrib.normals[3 * idx.normal_index + 2];

				newMesh->vertices.emplace_back(MRG::ColoredVertex{
				  .position = {posX, posY, posZ},
				  .normal   = {normX, normY, normZ},
				  .color    = {normX, normY, normZ},  // Set the color as the normal for now
				});
			}
			indexOffset += shapeVertexCount;
		}
	}

	return newMesh;
}

class SampleLayer : public MRG::Layer
{
public:
	void onAttach() override
	{
		mainCamera.position    = {0.f, 0.f, -3.f};
		mainCamera.aspectRatio = 1280.f / 720.f;
		mainCamera.setPerspective(glm::radians(70.f), 0.1f, 200.f);
		mainCamera.recalculateViewProjection();

		m_suzanne =
		  MRG::RenderObject<MRG::ColoredVertex>::create(loadFromFile("monkey_smooth.obj"), application->renderer.defaultColoredMaterial);
		m_boxy = MRG::RenderObject<MRG::ColoredVertex>::create(loadFromFile("boxy.obj"), application->renderer.defaultColoredMaterial);

		m_suzanne->translate({1.5f, 0.f, 0.f});
		m_suzanne->rotate({0.f, 1.f, 0.f}, glm::radians(180.f));
		m_suzanne->setVisible(false);
		m_boxy->translate({-1.5f, -1.5f, 0.f});

		application->renderer.uploadMesh(m_suzanne->mesh);
		application->renderer.uploadMesh(m_boxy->mesh);

		renderables.emplace_back(m_suzanne->getRenderData());
		renderables.emplace_back(m_boxy->getRenderData());
	}

	void onDetach() override { MRG_INFO("my final message. goodb ye") }
	void onUpdate(MRG::Timestep ts) override
	{
		m_elapsedTime += ts;
		application->renderer.clearColor.b = std::abs(std::sin(m_elapsedTime * 3.14f));
		m_suzanne->rotate({0.f, 1.f, 0.f}, ts.getSeconds() * glm::radians(180.f));
		m_boxy->rotate({0.f, 1.f, 0.f}, ts.getSeconds() * glm::radians(180.f));
	}
	void onEvent(MRG::Event& event) override
	{
		MRG::EventDispatcher dispatcher{event};
		dispatcher.dispatch<MRG::WindowResizeEvent>([this](MRG::WindowResizeEvent& event) { return mainCamera.onResize(event); });
		dispatcher.dispatch<MRG::KeyReleasedEvent>([this](MRG::KeyReleasedEvent&) {
			m_suzanne->setVisible(false);
			return true;
		});
		dispatcher.dispatch<MRG::KeyPressedEvent>([this](MRG::KeyPressedEvent&) {
			m_suzanne->setVisible(true);
			return true;
		});
	}

private:
	MRG::Ref<MRG::RenderObject<MRG::ColoredVertex>> m_suzanne{}, m_boxy{};
	float m_elapsedTime{};
};

class SampleApp : public MRG::Application
{
public:
	SampleApp()
	    : MRG::Application(MRG::ApplicationSpecification{.windowName            = "Morrigu sample app",
	                                                     .rendererSpecification = {.applicationName      = "SampleApp",
	                                                                               .windowWidth          = 1280,
	                                                                               .windowHeight         = 720,
	                                                                               .preferredPresentMode = vk::PresentModeKHR::eMailbox}})
	{
		pushLayer(new SampleLayer());
	};
};

int main()
{
	MRG::Logger::init();
	SampleApp application{};

	application.run();
}
