/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_TextureView_HPP___
#define ___VkRenderer_TextureView_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Image/TextureView.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Description d'une vue sur une image Vulkan.
	*\~english
	*\brief
	*	Vulkan image view wrapper.
	*/
	class TextureView
		: public renderer::TextureView
	{
	public:
		TextureView( Device const & device
			, Texture const & image
			, renderer::ImageViewCreateInfo const & createInfo );
		~TextureView();
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkImageView.
		*\~english
		*\brief
		*	VkImageView implicit cast operator.
		*/
		inline operator VkImageView const &()const
		{
			return m_view;
		}

	private:
		/**
		*\copydoc	renderer::TextureView::doMakeLayoutTransition
		*/
		renderer::ImageMemoryBarrier doMakeLayoutTransition( renderer::ImageLayout srcLayout
			, renderer::ImageLayout dstLayout
			, renderer::AccessFlags srcAccessFlags
			, renderer::AccessFlags dstAccessMask
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily )const override;

	private:
		Device const & m_device;
		VkImageView m_view{};
	};
}

#endif
