#ifndef MRG_VULKAN_IMPL_VERTEXARRAY
#define MRG_VULKAN_IMPL_VERTEXARRAY

#include "Renderer/APIs/Vulkan/Buffers.h"
#include "Renderer/APIs/Vulkan/VulkanHPPIncludeHelper.h"
#include "Renderer/VertexArray.h"

namespace MRG::Vulkan
{
	class VertexArray : public MRG::VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		void destroy() override
		{
			if (m_isDestroyed)
				return;

			for (auto& vb : m_vertexBuffers) vb->destroy();
			m_indexBuffer->destroy();

			m_isDestroyed = true;
		}

		void bind() const override;
		void unbind() const override;

		void addVertexBuffer(const Ref<MRG::VertexBuffer>& vertexBuffer) override;
		void setIndexBuffer(const Ref<MRG::IndexBuffer>& indexBuffer) override;

		[[nodiscard]] const std::vector<Ref<MRG::VertexBuffer>>& getVertexBuffers() const override { return m_vertexBuffers; };
		[[nodiscard]] const Ref<MRG::IndexBuffer>& getIndexBuffer() const override { return m_indexBuffer; };
		[[nodiscard]] auto getBindingDescription() const { return m_bindingDescription; }
		[[nodiscard]] auto getAttributeDescriptions() const { return m_attributeDescriptions; }

	private:
		std::vector<Ref<MRG::VertexBuffer>> m_vertexBuffers;
		Ref<MRG::IndexBuffer> m_indexBuffer;
		VkVertexInputBindingDescription m_bindingDescription{};
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
	};
}  // namespace MRG::Vulkan

#endif