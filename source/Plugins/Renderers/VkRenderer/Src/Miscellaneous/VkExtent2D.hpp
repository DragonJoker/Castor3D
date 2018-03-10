/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_Extent2D_HPP___
#define ___VkRenderer_Extent2D_HPP___
#pragma once

#include <Miscellaneous/Extent2D.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Extent2D en VkExtent2D.
	*\param[in] mode
	*	Le renderer::Extent2D.
	*\return
	*	Le VkExtent2D.
	*/
	VkExtent2D convert( renderer::Extent2D const & value );
}

#endif
