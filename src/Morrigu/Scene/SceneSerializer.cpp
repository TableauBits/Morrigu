#include "SceneSerializer.h"

#include "Core/GLMIncludeHelper.h"
#include "Core/Warnings.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_UNSAFE_FUNCTIONS
#include <yaml-cpp/yaml.h>
DISABLE_WARNING_POP

#include <filesystem>
#include <fstream>
#include <sstream>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const glm::vec3& v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec4& v)
	{
		out << Flow;
		out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
		return out;
	}
}  // namespace YAML

// clang-format off
struct SceneKeys
{
	static constexpr const char* key = "SceneName";
	struct Entities
	{
		static constexpr const char* key = "Entities";
		static constexpr const char* entityID = "EntityID";
		struct Tag
		{
			static constexpr const char* key = "TagComponent";
		};
		struct Transform
		{
			static constexpr const char* key = "TransformComponent";
			static constexpr const char* translation = "translation";
			static constexpr const char* rotation = "rotation";
			static constexpr const char* scale = "scale";

		};
		struct Camera
		{
			static constexpr const char* key = "CameraComponent";
			static constexpr const char* primary = "primary";
			static constexpr const char* fixedAspectRatio = "fixedAspectRatio";
			struct camera
			{
				static constexpr const char* key = "camera";
				static constexpr const char* projectionType = "projectionType";

				static constexpr const char* orthographicSize = "orthographicSize";
				static constexpr const char* orthographicNear = "orthographicNear";
				static constexpr const char* orthographicFar = "orthographicFar";

				static constexpr const char* perspectiveFOV = "perspectiveFOV";
				static constexpr const char* perspectiveNear = "perspectiveNear";
				static constexpr const char* perspectiveFar = "perspectiveFar";

			};
		};
		struct SpriteRenderer
		{
			static constexpr const char* key = "SpriteRendererComponent";
			static constexpr const char* color = "color";
		};
	};
};

namespace
{
	static void serializeEntity(YAML::Emitter& out, MRG::Entity entity)
	{
		out << YAML::BeginMap;
		{
			out << YAML::Key << SceneKeys::Entities::entityID << YAML::Value << "0000000000000000";  // TODO

			if (entity.hasComponent<MRG::TagComponent>())
				out << YAML::Key << SceneKeys::Entities::Tag::key << YAML::Value << entity.getComponent<MRG::TagComponent>().tag;

			if (entity.hasComponent<MRG::TransformComponent>()) {
				out << YAML::Key << SceneKeys::Entities::Transform::key << YAML::BeginMap;
				{
					auto& tc = entity.getComponent<MRG::TransformComponent>();
					out << YAML::Key << SceneKeys::Entities::Transform::translation << YAML::Value << tc.translation;
					out << YAML::Key << SceneKeys::Entities::Transform::rotation << YAML::Value << tc.rotation;
					out << YAML::Key << SceneKeys::Entities::Transform::scale << YAML::Value << tc.scale;
				}
				out << YAML::EndMap;
			}

			if (entity.hasComponent<MRG::CameraComponent>()) {
				out << YAML::Key << SceneKeys::Entities::Camera::key;
				out << YAML::BeginMap;
				{
					auto& cc = entity.getComponent<MRG::CameraComponent>();
					out << YAML::Key << SceneKeys::Entities::Camera::camera::key << YAML::Value << YAML::BeginMap;
					{
						out << YAML::Key << SceneKeys::Entities::Camera::camera::projectionType << YAML::Value << static_cast<int>(cc.camera.getProjectionType());

						out << YAML::Key << SceneKeys::Entities::Camera::camera::orthographicSize << YAML::Value << cc.camera.getOrthographicSize();
						out << YAML::Key << SceneKeys::Entities::Camera::camera::orthographicNear << YAML::Value << cc.camera.getOrthographicNear();
						out << YAML::Key << SceneKeys::Entities::Camera::camera::orthographicFar << YAML::Value << cc.camera.getOrthographicFar();

						out << YAML::Key << SceneKeys::Entities::Camera::camera::perspectiveFOV << YAML::Value << cc.camera.getPerspectiveFOV();
						out << YAML::Key << SceneKeys::Entities::Camera::camera::perspectiveNear << YAML::Value << cc.camera.getPerspectiveNear();
						out << YAML::Key << SceneKeys::Entities::Camera::camera::perspectiveFar << YAML::Value << cc.camera.getPerspectiveFar();
					}
					out << YAML::EndMap;

					out << YAML::Key << SceneKeys::Entities::Camera::primary << YAML::Value << cc.primary;
					out << YAML::Key << SceneKeys::Entities::Camera::fixedAspectRatio << YAML::Value << cc.fixedAspectRatio;
				}
				out << YAML::EndMap;
			}

			if (entity.hasComponent<MRG::SpriteRendererComponent>()) {
				out << YAML::Key << SceneKeys::Entities::SpriteRenderer::key << YAML::BeginMap;
				{
					auto& src = entity.getComponent<MRG::SpriteRendererComponent>();
					out << YAML::Key << SceneKeys::Entities::SpriteRenderer::color << YAML::Value << src.color;
				}
				out << YAML::EndMap;
			}
		}
		out << YAML::EndMap;
	}
}
// clang-format on

namespace MRG
{
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_scene(scene) {}

	void SceneSerializer::serialize(const std::string& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		{
			out << YAML::Key << SceneKeys::key << YAML::Value << "Untitled";  // TODO
			out << YAML::Key << SceneKeys::Entities::key << YAML::Value << YAML::BeginSeq;
			{
				m_scene->m_registry.each([&](auto& entityID) {
					Entity entity = {entityID, m_scene.get()};
					if (!entity)
						return;

					serializeEntity(out, entity);
				});
			}
			out << YAML::EndSeq;
		}
		out << YAML::EndMap;

		std::ofstream fout{filepath};
		fout << out.c_str();
	}

	bool SceneSerializer::deserialize(const std::string& filepath)
	{
		MRG_CORE_ASSERT(std::filesystem::exists(filepath), "file {} does not exist!", filepath);
		MRG_CORE_ASSERT(std::filesystem::is_regular_file(filepath), "file {} does not reference a file!", filepath);

		const auto data = YAML::LoadFile(filepath);
		if (!data[SceneKeys::key])
			return false;

		MRG_ENGINE_TRACE("Deserializing scene '{}'", data[SceneKeys::key].as<std::string>());

		auto entities = data[SceneKeys::Entities::key];
		if (!entities)
			return true;

		for (const auto& entity : entities) {
			const auto tag = entity[SceneKeys::Entities::Tag::key];
			std::string eName = tag ? tag.as<std::string>() : "Entity";

			MRG_ENGINE_TRACE("\tDeserialized entity '{}' {{ID{}}}", eName, entity[SceneKeys::Entities::entityID].as<uint64_t>());

			auto newEntity = m_scene->createEntity(eName);

			const auto transform = entity[SceneKeys::Entities::Transform::key];
			if (transform) {
				auto& tc = newEntity.getComponent<TransformComponent>();

				tc.translation = transform[SceneKeys::Entities::Transform::translation].as<glm::vec3>();
				tc.rotation = transform[SceneKeys::Entities::Transform::rotation].as<glm::vec3>();
				tc.scale = transform[SceneKeys::Entities::Transform::scale].as<glm::vec3>();
			}

			const auto camera = entity[SceneKeys::Entities::Camera::key];
			if (camera) {
				const auto cameraObj = camera[SceneKeys::Entities::Camera::camera::key];
				auto& cc = newEntity.addComponent<CameraComponent>();

				cc.camera.setOrthographic(cameraObj[SceneKeys::Entities::Camera::camera::orthographicSize].as<float>(),
				                          cameraObj[SceneKeys::Entities::Camera::camera::orthographicNear].as<float>(),
				                          cameraObj[SceneKeys::Entities::Camera::camera::orthographicFar].as<float>());

				cc.camera.setPerspective(cameraObj[SceneKeys::Entities::Camera::camera::perspectiveFOV].as<float>(),
				                         cameraObj[SceneKeys::Entities::Camera::camera::perspectiveNear].as<float>(),
				                         cameraObj[SceneKeys::Entities::Camera::camera::perspectiveFar].as<float>());

				cc.camera.setProjectionType(
				  static_cast<SceneCamera::ProjectionType>(cameraObj[SceneKeys::Entities::Camera::camera::projectionType].as<int>()));

				cc.primary = camera[SceneKeys::Entities::Camera::primary].as<bool>();
				cc.fixedAspectRatio = camera[SceneKeys::Entities::Camera::fixedAspectRatio].as<bool>();
			}

			const auto spriteRenderer = entity[SceneKeys::Entities::SpriteRenderer::key];
			if (spriteRenderer) {
				auto& src = newEntity.addComponent<SpriteRendererComponent>();

				src.color = spriteRenderer[SceneKeys::Entities::SpriteRenderer::color].as<glm::vec4>();
			}
		}

		return true;
	}

}  // namespace MRG
