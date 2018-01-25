/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::CommandPoolCreateFlags en VkCommandPoolCreateFlags.
	*\param[in] flags
	*	Le renderer::CommandPoolCreateFlags.
	*\return
	*	Le VkCommandPoolCreateFlags.
	*/
	VkCommandPoolCreateFlags convert( renderer::CommandPoolCreateFlags const & flags );
}
