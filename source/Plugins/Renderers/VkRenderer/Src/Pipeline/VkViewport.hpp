/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <Pipeline/Viewport.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Viewport en VkViewport.
	*\param[in] flags
	*	Le renderer::Viewport.
	*\return
	*	Le VkViewport.
	*/
	VkViewport convert( renderer::Viewport const & viewport );
}
