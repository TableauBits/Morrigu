//
// Created by Mathis Lamidey on 2021-05-29.
//

#include "Vertex.h"

#include "Rendering/RendererTypes.h"

namespace MRG
{
	VertexInputDescription BasicVertex::getVertexDescription()
	{
		vk::VertexInputBindingDescription mainBinding{
		  .binding   = 0,
		  .stride    = sizeof(BasicVertex),
		  .inputRate = vk::VertexInputRate::eVertex,
		};

		vk::VertexInputAttributeDescription position{
		  .location = 0,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(BasicVertex, position)),
		};
		vk::VertexInputAttributeDescription normal{
		  .location = 1,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(BasicVertex, normal)),
		};

		return VertexInputDescription{.bindings{mainBinding}, .attributes{position, normal}};
	}

	VertexInputDescription ColoredVertex::getVertexDescription()
	{
		vk::VertexInputBindingDescription mainBinding{
		  .binding   = 0,
		  .stride    = sizeof(ColoredVertex),
		  .inputRate = vk::VertexInputRate::eVertex,
		};

		vk::VertexInputAttributeDescription position{
		  .location = 0,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(ColoredVertex, position)),
		};
		vk::VertexInputAttributeDescription normal{
		  .location = 1,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(ColoredVertex, normal)),
		};
		vk::VertexInputAttributeDescription color{
		  .location = 2,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(ColoredVertex, color)),
		};

		return VertexInputDescription{.bindings{mainBinding}, .attributes{position, normal, color}};
	}

	VertexInputDescription TexturedVertex::getVertexDescription()
	{
		vk::VertexInputBindingDescription mainBinding{
		  .binding   = 0,
		  .stride    = sizeof(TexturedVertex),
		  .inputRate = vk::VertexInputRate::eVertex,
		};

		vk::VertexInputAttributeDescription position{
		  .location = 0,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(TexturedVertex, position)),
		};
		vk::VertexInputAttributeDescription normal{
		  .location = 1,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32B32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(TexturedVertex, normal)),
		};
		vk::VertexInputAttributeDescription texCoords{
		  .location = 2,
		  .binding  = 0,
		  .format   = vk::Format::eR32G32Sfloat,
		  .offset   = static_cast<uint32_t>(offsetof(TexturedVertex, texCoords)),
		};

		return VertexInputDescription{.bindings{mainBinding}, .attributes{position, normal, texCoords}};
	}
}  // namespace MRG
