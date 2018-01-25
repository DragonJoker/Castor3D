/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_ImageCopy_HPP___
#define ___VkRenderer_ImageCopy_HPP___
#pragma once

#include <Miscellaneous/ImageCopy.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageCopy en VkImageCopy.
	*\param[in] mode
	*	Le renderer::ImageCopy.
	*\return
	*	Le VkImageCopy.
	*/
	VkImageCopy convert( renderer::ImageCopy const & value );
}

#endif
