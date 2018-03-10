/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::SharingMode en VkSharingMode.
	*\param[in] value
	*	Le renderer::SharingMode.
	*\return
	*	Le VkSharingMode.
	*/
	VkSharingMode convert( renderer::SharingMode const & value );
}
