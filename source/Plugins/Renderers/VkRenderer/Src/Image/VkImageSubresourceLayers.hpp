/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_ImageSubresourceLayers_HPP___
#define ___VkRenderer_ImageSubresourceLayers_HPP___
#pragma once

#include <Image/ImageSubresourceLayers.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageSubresourceLayers en VkImageSubresourceLayers.
	*\param[in] value
	*	Le renderer::ImageSubresourceLayers.
	*\return
	*	Le VkImageSubresourceLayers.
	*/
	VkImageSubresourceLayers convert( renderer::ImageSubresourceLayers const & value );
}

#endif
