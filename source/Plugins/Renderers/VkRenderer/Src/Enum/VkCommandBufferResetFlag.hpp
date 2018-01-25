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
	*	Convertit un renderer::CommandBufferResetFlags en VkCommandBufferResetFlags.
	*\param[in] flags
	*	Le renderer::CommandBufferResetFlags.
	*\return
	*	Le VkCommandBufferResetFlags.
	*/
	VkCommandBufferResetFlags convert( renderer::CommandBufferResetFlags const & flags );
}
