/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::StencilOp en VkStencilOp.
	*\param[in] value
	*	Le renderer::StencilOp.
	*\return
	*	Le VkStencilOp.
	*/
	VkStencilOp convert( renderer::StencilOp const & value );
}
