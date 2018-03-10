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
	*	Convertit un renderer::ImageTiling en VkImageTiling.
	*\param[in] value
	*	Le renderer::ImageTiling.
	*\return
	*	Le VkImageTiling.
	*/
	VkImageTiling convert( renderer::ImageTiling const & value );
}
