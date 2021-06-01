//
// Created by Mathis Lamidey on 2021-04-11.
//

#include "VkInitialize.h"

namespace MRG::VkInit
{
	vk::CommandPoolCreateInfo cmdPoolCreateInfo(vk::CommandPoolCreateFlags flags, uint32_t queueFamiliyIndex)
	{
		return vk::CommandPoolCreateInfo{
		  .flags            = flags,
		  .queueFamilyIndex = queueFamiliyIndex,
		};
	}

	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(VkCommandPool pool, vk::CommandBufferLevel level, uint32_t count)
	{
		return vk::CommandBufferAllocateInfo{
		  .commandPool        = pool,
		  .level              = level,
		  .commandBufferCount = count,
		};
	}

	vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule)
	{
		return vk::PipelineShaderStageCreateInfo{
		  .stage  = stage,
		  .module = shaderModule,
		  .pName  = "main",
		};
	}

	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology topology)
	{
		return vk::PipelineInputAssemblyStateCreateInfo{
		  .topology               = topology,
		  .primitiveRestartEnable = VK_FALSE,
		};
	}

	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(vk::PolygonMode polygonMode)
	{
		return vk::PipelineRasterizationStateCreateInfo{
		  .depthClampEnable        = VK_FALSE,
		  .rasterizerDiscardEnable = VK_FALSE,
		  .polygonMode             = polygonMode,
		  .cullMode                = vk::CullModeFlagBits::eNone,  // @TODO(Ithyx): Change this to back when done debugging
		  .frontFace               = vk::FrontFace::eClockwise,
		  .depthBiasEnable         = VK_FALSE,
		  .lineWidth               = 1.f,
		};
	}

	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo()
	{
		return vk::PipelineMultisampleStateCreateInfo{
		  .rasterizationSamples = vk::SampleCountFlagBits::e1,
		  .sampleShadingEnable  = VK_FALSE,
		};
	}

	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState()
	{
		return vk::PipelineColorBlendAttachmentState{
		  .blendEnable         = VK_TRUE,
		  .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
		  .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
		  .colorBlendOp        = vk::BlendOp::eAdd,
		  .srcAlphaBlendFactor = vk::BlendFactor::eOne,
		  .dstAlphaBlendFactor = vk::BlendFactor::eZero,
		  .alphaBlendOp        = vk::BlendOp::eAdd,
		  .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
		                    vk::ColorComponentFlagBits::eA,
		};
	}

	vk::PipelineDepthStencilStateCreateInfo
	pipelineDepthStencilStateCreateInfo(bool shouldDepthTest, bool shouldDepthWrite, vk::CompareOp compareOp)
	{
		return vk::PipelineDepthStencilStateCreateInfo{
		  .depthTestEnable       = static_cast<vk::Bool32>(shouldDepthTest),
		  .depthWriteEnable      = static_cast<vk::Bool32>(shouldDepthWrite),
		  .depthCompareOp        = shouldDepthTest ? compareOp : vk::CompareOp::eAlways,
		  .depthBoundsTestEnable = VK_FALSE,
		  .stencilTestEnable     = VK_FALSE,
		  .minDepthBounds        = 0.f,
		  .maxDepthBounds        = 1.f,
		};
	}

	VkImageCreateInfo imageCreateInfo(vk::Format format, vk::ImageUsageFlags usage, vk::Extent3D extent)
	{
		return vk::ImageCreateInfo{
		  .imageType   = vk::ImageType::e2D,
		  .format      = format,
		  .extent      = extent,
		  .mipLevels   = 1,
		  .arrayLayers = 1,
		  .samples     = vk::SampleCountFlagBits::e1,
		  .tiling      = vk::ImageTiling::eOptimal,
		  .usage       = usage,
		};
	}

	vk::ImageViewCreateInfo imageViewCreateInfo(vk::Format format, vk::Image image, vk::ImageAspectFlags aspect)
	{
		return vk::ImageViewCreateInfo{
		  .image    = image,
		  .viewType = vk::ImageViewType::e2D,
		  .format   = format,
		  .subresourceRange =
		    {
		      .aspectMask     = aspect,
		      .baseMipLevel   = 0,
		      .levelCount     = 1,
		      .baseArrayLayer = 0,
		      .layerCount     = 1,
		    },
		};
	}
}  // namespace MRG::VkInit
