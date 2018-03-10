/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_ImageBlit_HPP___
#define ___VkRenderer_ImageBlit_HPP___
#pragma once

#include <Miscellaneous/ImageBlit.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ImageBlit en VkImageBlit.
	*\param[in] mode
	*	Le renderer::ImageBlit.
	*\return
	*	Le VkImageBlit.
	*/
	VkImageBlit convert( renderer::ImageBlit const & value );
}

#endif
