/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/StencilOpState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::StencilOpState en VkStencilOpState.
	*\param[in] state
	*	Le renderer::StencilOpState.
	*\return
	*	Le VkStencilOpState.
	*/
	VkStencilOpState convert( renderer::StencilOpState const & state );
}
