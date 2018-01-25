/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <Pipeline/Viewport.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::Viewport en vk::Viewport.
	*\param[in] flags
	*	Le renderer::Viewport.
	*\return
	*	Le vk::Viewport.
	*/
	VkViewport convert( renderer::Viewport const & viewport );
}
