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
	*	Convertit un renderer::RenderPassCreateFlags en VkRenderPassCreateFlags.
	*\param[in] flags
	*	Le renderer::RenderPassCreateFlags.
	*\return
	*	Le VkRenderPassCreateFlags.
	*/
	VkRenderPassCreateFlags convert( renderer::RenderPassCreateFlags const & flags );
	/**
	*\brief
	*	Convertit un VkRenderPassCreateFlags en renderer::RenderPassCreateFlags.
	*\param[in] flags
	*	Le VkRenderPassCreateFlags.
	*\return
	*	Le renderer::RenderPassCreateFlags.
	*/
	renderer::RenderPassCreateFlags convertRenderPassCreateFlags( VkRenderPassCreateFlags flags );
}
