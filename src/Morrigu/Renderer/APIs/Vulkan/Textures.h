#ifndef MRG_VULKAN_IMPL_TEXTURES
#define MRG_VULKAN_IMPL_TEXTURES

#include "Renderer/APIs/Vulkan/Helper.h"
#include "Renderer/Textures.h"

namespace MRG::Vulkan
{
	class Texture2D : public MRG::Texture2D
	{
	public:
		Texture2D(uint32_t width, uint32_t height);
		explicit Texture2D(const std::string& path);
		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) = delete;
		~Texture2D() override;

		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&&) = delete;

		void destroy() override;

		[[nodiscard]] uint32_t getWidth() const override { return m_width; };
		[[nodiscard]] uint32_t getHeight() const override { return m_height; };
		[[nodiscard]] ImTextureID getImTextureID() override;
		[[nodiscard]] VkImage& getHandle() { return m_imageHandle; }
		[[nodiscard]] VkDeviceMemory& getMemoryHandle() { return m_memoryHandle; }
		[[nodiscard]] static VkFormat getFormat() { return VK_FORMAT_R8G8B8A8_UNORM; }

		bool operator==(const Texture& other) const override { return m_imageHandle == ((Vulkan::Texture2D&)other).m_imageHandle; }

		void setData(void* data, uint32_t size) override;

		void bind(uint32_t slot) const override;

		[[nodiscard]] VkImageView getImageView() const { return m_imageView; };
		[[nodiscard]] VkSampler getSampler() const { return m_sampler; };

	private:
		ImTextureID m_ImTextureID = nullptr;

		VkImage m_imageHandle{};
		VkDeviceMemory m_memoryHandle{};
		VkImageView m_imageView{};
		VkSampler m_sampler{};
		uint32_t m_width, m_height;
	};
}  // namespace MRG::Vulkan

#endif
