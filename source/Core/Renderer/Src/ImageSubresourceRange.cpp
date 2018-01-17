/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "ImageSubresourceRange.hpp"

namespace renderer
{
	ImageSubresourceRange::ImageSubresourceRange( ImageAspectFlags aspectMask
		, uint32_t baseMipLevel
		, uint32_t levelCount
		, uint32_t baseArrayLayer
		, uint32_t layerCount )
		: m_aspectMask{ aspectMask }
		, m_baseMipLevel{ baseMipLevel }
		, m_levelCount{ levelCount }
		, m_baseArrayLayer{ baseArrayLayer }
		, m_layerCount{ layerCount }
	{
	}
}
