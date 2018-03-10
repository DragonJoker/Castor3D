/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_Offset2D_HPP___
#define ___VkRenderer_Offset2D_HPP___
#pragma once

#include <Miscellaneous/Offset2D.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Offset2D en VkOffset2D.
	*\param[in] mode
	*	Le renderer::Extent3D.
	*\return
	*	Le VkOffset2D.
	*/
	VkOffset2D convert( renderer::Offset2D const & value );
}

#endif
