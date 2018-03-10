/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_Offset3D_HPP___
#define ___VkRenderer_Offset3D_HPP___
#pragma once

#include <Miscellaneous/Offset3D.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Offset3D en VkOffset3D.
	*\param[in] mode
	*	Le renderer::Offset3D.
	*\return
	*	Le VkOffset3D.
	*/
	VkOffset3D convert( renderer::Offset3D const & value );
}

#endif
