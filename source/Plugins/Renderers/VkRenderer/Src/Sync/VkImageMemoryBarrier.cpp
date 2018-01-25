#include "VkRendererPrerequisites.hpp"

#include "Image/VkTexture.hpp"

namespace vk_renderer
{
	VkImageMemoryBarrier convert( renderer::ImageMemoryBarrier const & barrier )
	{
		return VkImageMemoryBarrier
		{
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			convert( barrier.getSrcAccessMask() ),
			convert( barrier.getDstAccessMask() ),
			convert( barrier.getOldLayout() ),
			convert( barrier.getNewLayout() ),
			barrier.getSrcQueueFamilyIndex(),
			barrier.getDstQueueFamilyIndex(),
			static_cast< Texture const & >( barrier.getImage() ),
			convert( barrier.getSubresourceRange() )
		};
	}
}
