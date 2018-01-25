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
	*	Convertit un renderer::BlendFactor en VkBlendFactor.
	*\param[in] value
	*	Le renderer::BlendFactor.
	*\return
	*	Le VkBlendFactor.
	*/
	VkBlendFactor convert( renderer::BlendFactor const & value );
}
