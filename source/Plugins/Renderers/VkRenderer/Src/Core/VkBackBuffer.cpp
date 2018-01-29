/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkBackBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Miscellaneous/VkMemoryStorage.hpp"

namespace vk_renderer
{
	BackBuffer::BackBuffer( Device const & device
		, renderer::SwapChain const & swapChain
		, uint32_t imageIndex
		, renderer::PixelFormat format
		, renderer::UIVec2 const & dimensions
		, Texture && texture )
		: renderer::BackBuffer{ device, swapChain, imageIndex }
		, m_image{ std::move( texture ) }
		, m_view{ device
			, m_image
			, m_image.getType()
			, format
			, 0u
			, 1u
			, 0u
			, 1u
			, renderer::ImageLayout::ePresentSrc
			, renderer::AccessFlag::eMemoryRead }
	{
	}
}
